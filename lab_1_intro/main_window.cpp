#include <QMouseEvent>

#include <opencv2/opencv.hpp>

#include <icpl/utils.h>

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

void MainWindow::build_hist(const QImage &image) {
    const int HIST_WIDTH = width() - main_image_label->width() - 4 * GAP;
    const int HIST_HEIGHT = std::min(HIST_WIDTH, height()) * 0.85;
    hist_image_label->move(main_image_label->width() + 2 * GAP, GAP + 20);
    hist_image_label->resize(HIST_WIDTH, HIST_HEIGHT);
    assert(main_image_label->width() + HIST_WIDTH + 4 * GAP <= width());


    cv::Mat cv_main_image = gui::cvmat_from_qimage(image);
    auto histograms = icpl::build_histograms(cv_main_image);
    cv::Mat main_image_hist = icpl::draw_histogram(histograms,
                                                   {cv::Scalar(255, 0, 0),
                                                    cv::Scalar(0, 255, 0),
                                                    cv::Scalar(0, 0, 255)},
                                                   HIST_HEIGHT, HIST_WIDTH);

    QImage hist_image = gui::cvmat_to_qimage(main_image_hist);
    hist_image_label->setPixmap(QPixmap::fromImage(hist_image));
}

void MainWindow::set_main_image(const QImage &new_image) {
    BaseWindow::set_main_image(new_image);

    if (!main_image.isNull()) {
        build_hist(main_image);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    BaseWindow::resizeEvent(event);

    if (!snd_point.isNull() && !fst_point.isNull()) {
        show_for_area();
        return;
    }

    auto cur_pixmap = main_image_label->pixmap();
    if (cur_pixmap != nullptr && !cur_pixmap->isNull()) {
        build_hist(cur_pixmap->toImage());
    }
}


QPoint MainWindow::get_image_point(const QPoint &gl_point) {
    int gl_x = gl_point.x();
    int gl_y = gl_point.y();

    int im_gl_x = main_image_label->pos().x();
    int im_gl_y = main_image_label->pos().y();
    int im_w = main_image_label->width();
    int im_h = main_image_label->height();

    if (gl_x >= im_gl_x && gl_y >= im_gl_y &&
        gl_x <= im_gl_x + im_w &&
        gl_y <= im_gl_y + im_h)
    {
        int src_x = cvRound((gl_x - im_gl_x) / (float)im_w * main_image.width());
        int src_y = cvRound((gl_y - im_gl_y) / (float)im_h * main_image.height());
        return QPoint(src_x, src_y);
    } else {
        return QPoint();
    }
}

void MainWindow::show_for_area() {
    if (!fst_point.isNull() && !snd_point.isNull()) {
        cv::Mat cv_main_image = gui::cvmat_from_qimage(main_image);
        cv::Rect roi(cv::Point(fst_point.x(), fst_point.y()),
                     cv::Point(snd_point.x(), snd_point.y()));
        auto cv_subim = cv_main_image(roi);

        cv_subim += 70;

        auto temp = main_image;
        auto qt_image = gui::cvmat_to_qimage(cv_main_image);
        auto hist_image = gui::cvmat_to_qimage(cv_subim);

        set_main_image(qt_image);
        build_hist(hist_image);
        main_image = temp;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    BaseWindow::mousePressEvent(event);

    if (event->button() == Qt::LeftButton) {
        fst_point = event->pos();
    } else {
        fst_point = QPoint();
        snd_point = QPoint();

        set_main_image(main_image);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    BaseWindow::mousePressEvent(event);

    if (event->button() == Qt::LeftButton && event->pos() != fst_point) {
        snd_point = event->pos();

        fst_point = get_image_point(fst_point);
        snd_point = get_image_point(snd_point);

        show_for_area();
    }
}
