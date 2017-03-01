#ifndef INTRO_WINDOW_H
#define INTRO_WINDOW_H

#include <QMainWindow>

#include <QImage>
#include <QLabel>
#include <QPushButton>

class IntroWindow : public QMainWindow
{
    Q_OBJECT

public:
    IntroWindow();
    ~IntroWindow();

private:
    QImage image;
    QLabel *image_label;

    QPushButton load_button;
    QPushButton save_button;
};

#endif // INTRO_WINDOW_H
