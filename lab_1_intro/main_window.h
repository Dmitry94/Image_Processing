#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QGroupBox>
#include <QVBoxLayout>

#include <base_window.h>

class MainWindow : public gui::BaseWindow {
public:
    MainWindow();
    virtual ~MainWindow();

protected:
    /**
     * Callback for resize window.
     *
     * @param event[in] Event params.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * Callback for mouse button click/release.
     *
     * @param event[in] Event params.
     */
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * Sets main image, and also histogram.
     *
     * @param new_image[in] New image.
     */
    void set_main_image(const QImage &new_image) override;

private:
    /** Histogram image and it's container */
    QLabel *hist_image_label;

    /** Point for area */
    QPoint fst_point;
    QPoint snd_point;

    /**
     * Get point from window to original image.
     *
     * @param gl_point[in]  Window coordinates.
     *
     * @return image coordinates.
     */
    QPoint get_image_point(const QPoint &gl_point);

    /**
     * Shows histogram for area and show this area on the picture.
     */
    void show_for_area();

    /** Build hist from image */
    void build_hist(const QImage &image);
};

#endif // MAIN_WINDOW_H
