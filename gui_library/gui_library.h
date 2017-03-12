#ifndef GUI_LIBRARY_H
#define GUI_LIBRARY_H

#include <QImage>
#include <QPixmap>

#include <opencv2/core/core.hpp>

namespace gui {

/**
 * Creates QImage object based on cv::Mat.
 * Works only with CV8UC1 - CV8UC4 types.
 *
 * @param image[in] Input image.
 *
 * @return QImage object.
 */
QImage cvmat_to_qimage(const cv::Mat& image);


/**
 * Creates QPixmap object based on cv::Mat.
 * Works only with CV8UC1 - CV8UC4 types.
 *
 * @param image[in] Input image.
 *
 * @return QPixmap object.
 */
QPixmap cvmat_to_qpixmap(const cv::Mat& image);

/**
 * Creates cv::Mat object from QImage.
 *
 * @param qimage    QImage object.
 *
 * @return cv::Mat clone.
 */
cv::Mat cvmat_from_qimage(const QImage& qimage);

/**
 * Creates cv::Mat object from QPixmap.
 *
 * @param qpixmap   QPixmap object.
 *
 * @return cv::Mat clone.
 */
cv::Mat cvmat_from_qpixmap(const QPixmap& qpixmap);

} // namespace gui


#endif // GUI_LIBRARY_H
