#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    /**
     * Callback from resize window.
     *
     * @param event[in] Resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

private:
    /** UI from form */
    Ui::MainWindow *ui;

    /** Source image */
    QImage source_image;

    /**
     * Creates menu actions.
     */
    virtual void create_actions();

    /**
     * Updates menu actions.
     */
    virtual void update_actions();

    /**
     * Sets main image, and also histogram.
     *
     * @param new_image[in] New image.
     */
    void set_source_image(const QImage &new_image);

    /**
     * Loads main image from file.
     *
     * file_name[in]    String, describes filename.
     *
     * @return Success indicator.
     */
    bool load_image(const QString &file_name);

    /**
     * Saves main image to file.
     *
     * @param file_name[in] String, describes filename.
     *
     * @return Success indicator.
     */
    bool save_image(const QString &file_name);

    /**
     * Init filedialog for work with images.
     *
     * @param dialog[in]         File dialog.
     * @param accept_mode[in]    Accept mode.
     */
    void init_image_file_dialog(QFileDialog &dialog,
                                QFileDialog::AcceptMode accept_mode);

private slots:
    void open();
    void save_as();
};

#endif // MAINWINDOW_H
