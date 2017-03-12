#include <QtWidgets>

#include <opencv2/core/core.hpp>

#include <icpl.h>

#include <gui_library.h>

#include "main_window.h"


MainWindow::MainWindow() : BaseWindow() {
    width_koef = 3.0 / 5.0;
    height_koef = 1.0;

    hist_image_label = new QLabel(this);
}

MainWindow::~MainWindow() {
    delete hist_image_label;
}

void MainWindow::set_main_image(const QImage &new_image) {
    BaseWindow::set_main_image(new_image);

    if (new_image.isNull()) {
        hist_image_label->resize(0, 0);
        return;
    }

    const int HIST_WIDTH = width() - main_image_label->width() - 4 * GAP;
    const int HIST_HEIGHT = HIST_WIDTH * 0.85;

    hist_image_label->move(main_image_label->width() + 2 * GAP, GAP + 20);
    hist_image_label->resize(HIST_WIDTH, HIST_HEIGHT);


    cv::Mat hist_vis(HIST_HEIGHT, HIST_WIDTH, CV_8UC3, cv::Scalar(0));
    cv::Mat cv_main_image = gui::cvmat_from_qimage(new_image);
    auto histograms = icpl::build_histograms(cv_main_image);

    std::vector<cv::Scalar> colors { cv::Scalar(255, 0, 0),
                                     cv::Scalar(0, 255, 0),
                                     cv::Scalar(0, 0, 255) };

    for (size_t i = 0; i < histograms.size(); i++) {
        icpl::draw_histogram(histograms[i], colors[i % colors.size()],
                             hist_vis);
    }

    hist_image = gui::cvmat_to_qimage(hist_vis);
    hist_image_label->setPixmap(QPixmap::fromImage(hist_image));
}

void MainWindow::resizeEvent(QResizeEvent* event) {
   BaseWindow::resizeEvent(event);
}
