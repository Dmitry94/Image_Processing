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

    enum class Transformation { REFERENCE_COLORS };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    /**
     * Callback for mouse button click/release.
     *
     * @param event[in] Event params.
     */
    void mousePressEvent(QMouseEvent *event) override;

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

    /** Transformation */
    Transformation transformation;

    /** Reference colors for correction */
    QPoint src_color_point;
    QPoint dst_color_point;

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

    /** Apply transformation and show result */
    void apply_transform();

    /**
     * Get point from window to original image.
     *
     * @param gl_point[in]  Window coordinates.
     *
     * @return image coordinates.
     */
    QPoint get_image_point(const QPoint &gl_point);

private slots:
    void open();
    void save_as();

    void reference_correction_choose();
};

#endif // MAINWINDOW_H
