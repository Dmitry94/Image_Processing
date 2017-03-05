#ifndef INTRO_WINDOW_H
#define INTRO_WINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QLabel>

class IntroWindow : public QMainWindow
{
    Q_OBJECT

public:
    IntroWindow();
    virtual ~IntroWindow();

    bool load_file(const QString &);

private slots:
    void open();
    void save_as();

private:
    void create_actions();
    void update_actions();
    bool save_file(const QString &file_name);
    void set_image(const QImage &new_image);

    void init_image_file_dialog(QFileDialog &dialog,
                                QFileDialog::AcceptMode acceptMode);

    QImage image;
    QLabel *image_label;

    QAction *open_act;
    QAction *save_act;
};

#endif // INTRO_WINDOW_H
