#include "intro_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    IntroWindow w;
    w.setWindowTitle("Intro application");
    w.show();

    return a.exec();
}
