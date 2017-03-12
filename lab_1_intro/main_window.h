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
    void resizeEvent(QResizeEvent *event) override;
    void set_main_image(const QImage &new_image) override;

private:
    QImage hist_image;
    QLabel *hist_image_label;

};

#endif // MAIN_WINDOW_H
