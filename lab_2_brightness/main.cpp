#include <QApplication>

#include <main_window.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("Lab 2 application");
    w.show();

    return a.exec();
}
