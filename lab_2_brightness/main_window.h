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

private slots:
    void fix_color_correct();
    void gray_world();
    void gamma_correct();
    void contrast_correct();
    void hist_normalization();
    void hist_equalization();

private:
    /** Result image and it's container */
    QImage result_image;
    QLabel *result_image_label;
};

#endif // MAIN_WINDOW_H
