#include <QMouseEvent>

#include <opencv2/opencv.hpp>

#include <icpl/utils.h>

#include <gui_library.h>

#include "main_window.h"


MainWindow::MainWindow() : BaseWindow() {
    width_koef = 1.0 / 2.0;
    height_koef = 1.0;

    result_image_label = new QLabel(this);
}

MainWindow::~MainWindow() {
    delete result_image_label;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    BaseWindow::resizeEvent(event);

    const int RES_WIDTH = width() - main_image_label->width() - 4 * GAP;
    const int RES_HEIGHT = std::min(RES_WIDTH, height()) * 0.85;
    result_image_label->move(main_image_label->width() + 2 * GAP, GAP + 20);
    result_image_label->resize(RES_WIDTH, RES_HEIGHT);
}
