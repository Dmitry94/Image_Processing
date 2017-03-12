#include <QtWidgets>

#include "base_window.h"

namespace gui {

BaseWindow::BaseWindow() {
    move(100, 100);
    resize(QGuiApplication::primaryScreen()->availableSize() * 4 / 5);
    create_actions();

    main_image_label = new QLabel(this);
    main_image_label->move(GAP, GAP + 20);
}

BaseWindow::~BaseWindow() {
    delete main_image_label;
    delete open_act;
    delete save_act;
}


bool BaseWindow::load_image(const QString &file_name) {
    QImageReader reader(file_name);
    reader.setAutoTransform(true);
    const QImage new_image = reader.read();
    if (new_image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(file_name), reader.errorString()));
        return false;
    }

    set_main_image(new_image);
    setWindowFilePath(file_name);

    return true;
}

void BaseWindow::set_main_image(const QImage &new_image) {
    main_image = new_image;
    QImage window_image = new_image.scaled((width() - 2 * GAP) * width_koef,
                                           (height()- 4 * GAP) * height_koef,
                                           Qt::KeepAspectRatio);

    main_image_label->resize(window_image.size());
    main_image_label->setPixmap(QPixmap::fromImage(window_image));
    update_actions();
}


bool BaseWindow::save_image(const QString &file_name) {
    QImageWriter writer(file_name);

    if (!writer.write(main_image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(file_name)),
                                 writer.errorString());
        return false;
    }

    return true;
}


void BaseWindow::init_image_file_dialog(QFileDialog &dialog,
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

void BaseWindow::open() {
    QFileDialog dialog(this, tr("Open File"));
    init_image_file_dialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted &&
           !load_image(dialog.selectedFiles().first())) {}
}

void BaseWindow::save_as() {
    QFileDialog dialog(this, tr("Save File As"));
    init_image_file_dialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted &&
           !save_image(dialog.selectedFiles().first())) {}
}


void BaseWindow::create_actions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    open_act = fileMenu->addAction(tr("&Open..."), this, &BaseWindow::open);
    open_act->setShortcut(QKeySequence::Open);

    save_act = fileMenu->addAction(tr("&Save As..."), this, &BaseWindow::save_as);
    save_act->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));
}

void BaseWindow::update_actions() {
    save_act->setEnabled(!main_image.isNull());
}

void BaseWindow::resizeEvent(QResizeEvent* event) {
   QMainWindow::resizeEvent(event);
   this->set_main_image(main_image);
}

}
