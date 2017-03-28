#include <QResizeEvent>
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
    move(100, 100);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::set_source_image(const QImage &new_image) {
    source_image = new_image;

    auto new_w = ui->l_source->width();
    auto new_h = ui->l_source->height();
    auto resized_src_im = source_image.scaled(new_w, new_h);
    ui->l_source->setPixmap(QPixmap::fromImage(resized_src_im));

    update_actions();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (!source_image.isNull()) {
        auto new_w = ui->l_source->width();
        auto new_h = ui->l_source->height();
        auto resized_src_im = source_image.scaled(new_w, new_h);
        ui->l_source->setPixmap(QPixmap::fromImage(resized_src_im));
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
}

void MainWindow::update_actions() {
    ui->actionSave_as->setEnabled(!source_image.isNull());
}
