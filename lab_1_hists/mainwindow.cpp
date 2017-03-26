#include <QMouseEvent>
#include <QtWidgets>

#include <opencv2/opencv.hpp>

#include <icpl/utils.h>

#include <gui_library.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    create_actions();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::build_hist(const QImage &image) {
    cv::Mat src_image_cv = gui::cvmat_from_qimage(image);
    auto histograms = icpl::build_histograms(src_image_cv);
    cv::Mat hist_image_cv = icpl::draw_histogram(histograms,
                                                {cv::Scalar(255, 0, 0),
                                                 cv::Scalar(0, 255, 0),
                                                 cv::Scalar(0, 0, 255)},
                                                 source_image.height(),
                                                 source_image.height() * 1.25);

    QImage hist_image = gui::cvmat_to_qimage(hist_image_cv);
    ui->hist_label->setPixmap(QPixmap::fromImage(hist_image));
}

void MainWindow::set_source_image(const QImage &new_image) {
    source_image = new_image;

    ui->src_image_label->setPixmap(QPixmap::fromImage(source_image));
    build_hist(source_image);

    update_actions();
}

QPoint MainWindow::get_image_point(const QPoint &gl_point) {
    int gl_x = gl_point.x();
    int gl_y = gl_point.y();

    int im_gl_x = ui->src_image_label->pos().x();
    int im_gl_y = ui->src_image_label->pos().y();
    int im_w = ui->src_image_label->width();
    int im_h = ui->src_image_label->height();

    if (gl_x >= im_gl_x && gl_y >= im_gl_y &&
        gl_x <= im_gl_x + im_w &&
        gl_y <= im_gl_y + im_h)
    {
        int src_x = cvRound((gl_x - im_gl_x) / (float)im_w * source_image.width());
        int src_y = cvRound((gl_y - im_gl_y) / (float)im_h * source_image.height());
        return QPoint(src_x, src_y);
    } else {
        return QPoint();
    }
}

void MainWindow::show_for_area() {
    if (!fst_point.isNull() && !snd_point.isNull()) {
        cv::Mat cv_main_image = gui::cvmat_from_qimage(source_image);
        cv::Rect roi(cv::Point(fst_point.x(), fst_point.y()),
                     cv::Point(snd_point.x(), snd_point.y()));

        auto cv_subim = cv_main_image(roi);
        auto hist_image = gui::cvmat_to_qimage(cv_subim);

        cv_subim = cv_subim * 0.7 + cv::Scalar(0, 255, 0) * 0.3;
        auto temp = source_image;
        auto qt_image = gui::cvmat_to_qimage(cv_main_image);

        set_source_image(qt_image);
        build_hist(hist_image);
        source_image = temp;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QMainWindow::mousePressEvent(event);

    if (event->button() == Qt::LeftButton) {
        fst_point = event->pos();
    } else {
        fst_point = QPoint();
        snd_point = QPoint();

        set_source_image(source_image);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    QMainWindow::mousePressEvent(event);

    if (event->button() == Qt::LeftButton && event->pos() != fst_point) {
        snd_point = event->pos();

        fst_point = get_image_point(fst_point);
        snd_point = get_image_point(snd_point);

        show_for_area();
    }
}

bool MainWindow::load_image(const QString &file_name) {
    QImageReader reader(file_name);
    reader.setAutoTransform(true);
    const QImage new_image = reader.read();
    if (new_image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(file_name), reader.errorString()));
        return false;
    }

    set_source_image(new_image);
    setWindowFilePath(file_name);

    return true;
}

bool MainWindow::save_image(const QString &file_name) {
    QImageWriter writer(file_name);

    if (!writer.write(source_image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(file_name)),
                                 writer.errorString());
        return false;
    }

    return true;
}


void MainWindow::init_image_file_dialog(QFileDialog &dialog,
                                         QFileDialog::AcceptMode accept_mode) {
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations
                = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath()
                                                        : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = accept_mode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (accept_mode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void MainWindow::open() {
    QFileDialog dialog(this, tr("Open File"));
    init_image_file_dialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted &&
           !load_image(dialog.selectedFiles().first())) {}
}

void MainWindow::save_as() {
    QFileDialog dialog(this, tr("Save File As"));
    init_image_file_dialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted &&
           !save_image(dialog.selectedFiles().first())) {}
}


void MainWindow::create_actions() {
    ui->actionOpen->setShortcut(QKeySequence::Open);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));

    ui->actionSave_as->setEnabled(false);
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(save_as()));

    ui->actionExit->setShortcut(tr("Ctrl+Q"));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::update_actions() {
    ui->actionSave_as->setEnabled(!source_image.isNull());
}
