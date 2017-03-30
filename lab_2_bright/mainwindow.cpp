#include <QResizeEvent>
#include <QMouseEvent>
#include <QtWidgets>

#include <opencv2/opencv.hpp>

#include <icpl/brightness_mapings.h>

#include <gui_library.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    create_actions();
    update_actions();
    move(100, 100);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::set_source_image(const QImage &new_image) {
    source_image = new_image;
    transformation = Transformation::NONE;

    auto new_w = ui->l_source->width();
    auto new_h = ui->l_source->height();
    auto resized_src_im = source_image.scaled(new_w, new_h);
    ui->l_source->setPixmap(QPixmap::fromImage(resized_src_im));
    ui->l_result->setPixmap(QPixmap());

    update_actions();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (!source_image.isNull()) {
        auto new_w = ui->l_source->width();
        auto new_h = ui->l_source->height();
        auto resized_src_im = source_image.scaled(new_w, new_h);
        ui->l_source->setPixmap(QPixmap::fromImage(resized_src_im));
        apply_transform();
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

    dialog.setDirectory("../../content");
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


    connect(ui->actionCorrection_with_reference_colors, SIGNAL(triggered()),
            this, SLOT(reference_correction_choose()));
    connect(ui->actionGray_world, SIGNAL(triggered()),
            this, SLOT(gray_world_choose()));
    connect(ui->actionGamma_correction, SIGNAL(triggered()),
            this, SLOT(gamma_correction_choose()));
    connect(ui->actionContrast_correction, SIGNAL(triggered()),
            this, SLOT(contrast_correction_choose()));
    connect(ui->actionHistogram_normalization, SIGNAL(triggered()),
            this, SLOT(hist_norm_choose()));
    connect(ui->actionHistogram_equalization, SIGNAL(triggered()),
            this, SLOT(hist_eq_choose()));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), this,
            SLOT(slider_value_changed(int)));
}

void MainWindow::update_actions() {
    ui->actionSave_as->setEnabled(!source_image.isNull());
    ui->horizontalSlider->setHidden(
                transformation != Transformation::GAMMA_CORRECTION &&
                transformation != Transformation::CONTRAST_CORRECTION
            );
}

/** Correction slots */
void MainWindow::reference_correction_choose() {
    transformation = Transformation::REFERENCE_COLORS;
    update_actions();
    apply_transform();
}

void MainWindow::gray_world_choose() {
    transformation = Transformation::GRAY_WORLD;
    update_actions();
    apply_transform();
}

void MainWindow::gamma_correction_choose() {
    transformation = Transformation::GAMMA_CORRECTION;
    update_actions();
    ui->horizontalSlider->setValue(50);
    apply_transform();
}

void MainWindow::contrast_correction_choose() {
    transformation = Transformation::CONTRAST_CORRECTION;
    update_actions();
    ui->horizontalSlider->setValue(50);
    apply_transform();
}

void MainWindow::hist_norm_choose() {
    transformation = Transformation::HIST_NORMALIZATION;
    update_actions();
    apply_transform();
}

void MainWindow::hist_eq_choose() {
    transformation = Transformation::HIST_EQUALIZATION;
    update_actions();
    apply_transform();
}

void MainWindow::slider_value_changed(int) {
    if (transformation == Transformation::GAMMA_CORRECTION ||
        transformation == Transformation::CONTRAST_CORRECTION) {
        apply_transform();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QMainWindow::mousePressEvent(event);

    if (transformation == Transformation::REFERENCE_COLORS) {
        if (event->button() == Qt::LeftButton) {
            src_color_point = event->pos();
        } else if (event->button() == Qt::RightButton) {
            dst_color_point = event->pos();
        }

        apply_transform();
    }
}

void MainWindow::apply_transform() {
    if (source_image.isNull()) {
        return;
    }
    cv::Mat src_image_cv = gui::cvmat_from_qimage(source_image);
    cv::Mat result;

    switch (transformation) {
        case Transformation::NONE: {
            result = src_image_cv;
            break;
        }

        case Transformation::REFERENCE_COLORS: {
            if (src_color_point.isNull() || dst_color_point.isNull()) {
                result = src_image_cv;
                return;
            }
            auto src_image_point = get_image_point(src_color_point);
            auto dst_image_point = get_image_point(dst_color_point);

            cv::Scalar src_color(0), dst_color(0);
            if (src_image_cv.channels() == 3) {
                src_color = src_image_cv.at<cv::Vec3b>(src_image_point.y(),
                                                       src_image_point.x());
                dst_color = src_image_cv.at<cv::Vec3b>(dst_image_point.y(),
                                                       dst_image_point.x());
            } else if (src_image_cv.channels() == 1) {
                src_color = src_image_cv.at<uchar>(src_image_point.y(),
                                                   src_image_point.x());
                dst_color = src_image_cv.at<uchar>(dst_image_point.y(),
                                                   dst_image_point.x());
            } else if (src_image_cv.channels() == 4) {
                src_color = src_image_cv.at<cv::Vec4b>(src_image_point.y(),
                                                       src_image_point.x());
                dst_color = src_image_cv.at<cv::Vec4b>(dst_image_point.y(),
                                                       dst_image_point.x());
            } else {
                throw std::logic_error("apply_transform: Channels aren't good!");
            }

            result = icpl::correct_with_reference_colors(src_image_cv,
                                                         src_color, dst_color);
            break;
        }


        case Transformation::GRAY_WORLD: {
            result = icpl::apply_gray_world_effect(src_image_cv);
            break;
        }


        case Transformation::GAMMA_CORRECTION: {
            result = icpl::apply_gamma_correction(src_image_cv,
                                                  ui->horizontalSlider->value());
            break;
        }

        case Transformation::CONTRAST_CORRECTION: {
            result = icpl::apply_contrast_correction(src_image_cv,
                                                     ui->horizontalSlider->value());
            break;
        }

        case Transformation::HIST_NORMALIZATION: {
            result = icpl::apply_hist_normalization(src_image_cv, 2, 3);
            break;
        }

        case Transformation::HIST_EQUALIZATION: {
            result = icpl::apply_hist_equalization(src_image_cv);
            break;
        }
    }

    QImage result_qimage = gui::cvmat_to_qimage(result);
    auto new_w = ui->l_result->width();
    auto new_h = ui->l_result->height();
    auto resized_result = result_qimage.scaled(new_w, new_h);
    ui->l_result->setPixmap(QPixmap::fromImage(resized_result));
}

QPoint MainWindow::get_image_point(const QPoint &gl_point) {
    int gl_x = gl_point.x();
    int gl_y = gl_point.y();

    int im_gl_x = ui->l_source->pos().x();
    int im_gl_y = ui->l_source->pos().y();
    int im_w = ui->l_source->width();
    int im_h = ui->l_source->height();

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
