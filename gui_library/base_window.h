#ifndef BASE_WINDOW_H
#define BASE_WINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QLabel>

namespace gui {

/**
 * Base class for windows with images.
 */
class BaseWindow : public QMainWindow
{
    Q_OBJECT

public:
    BaseWindow();
    virtual ~BaseWindow();

protected:
    /**
     * Set new main image
     *
     * @param new_image[in] New image.
     */
    virtual void set_main_image(const QImage &new_image);

    /**
     * Creates menu actions.
     */
    virtual void create_actions();

    /**
     * Updates menu actions.
     */
    virtual void update_actions();

    /**
     * Callback for resize window.
     *
     * @param event[in] Event params.
     */
    void resizeEvent(QResizeEvent *event) override;

    /** Main image and its container in Window */
    QImage main_image;
    QLabel *main_image_label;

    /** Koefs on which image sizes will be mult */
    float width_koef = 1.0;
    float height_koef = 1.0;

    /** Gap from top, bot, left and right */
    const int GAP = 10;

private slots:
    void open();
    void save_as();

private:
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


    QAction *open_act;
    QAction *save_act;
};

} //namespace gui

#endif // BASE_WINDOW_H
