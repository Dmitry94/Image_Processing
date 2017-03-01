#include <QScreen>

#include "intro_window.h"
#include "ui_intro_window.h"

IntroWindow::IntroWindow() {
    image = QImage();

//    load_button = QPushButton();
//    save_button = QPushButton();

    move(100, 100);
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    setWindowTitle("Intro application");
}

IntroWindow::~IntroWindow() { }
