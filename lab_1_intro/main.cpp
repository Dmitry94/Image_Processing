#include <QApplication>

#include <base_window.h>

#include <icpl.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    gui::BaseWindow w;
    w.setWindowTitle("Base application");
    w.show();

    auto hist = icpl::build_histogram(cv::Mat());

    return a.exec();
}
