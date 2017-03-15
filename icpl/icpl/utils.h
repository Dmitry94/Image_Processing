#ifndef UTILS_H
#define UTILS_H

#include <vector>

#include <opencv2/core/core.hpp>

namespace icpl {

/**
 * Builds histograms array for each channel in src image.
 * Works only with CV_8U type.
 *
 * @param src[in]   Input image.
 *
 * @return vector of histograms, each of is vector of float.
 */
std::vector<std::vector<float>> build_histograms(const cv::Mat &src);


/**
 * Creates histograms image.
 *
 * @param histograms[in]    Array of histograms.
 * @param colors[in]        Color for each array-line.
 * @param height[in]        Out image height.
 * @param width[in]         Out image width.
 *
 * @return image representation.
 */
cv::Mat draw_histogram(const std::vector<std::vector<float>> &histograms,
                       const std::vector<cv::Scalar> &colors,
                       const int height, const int width);

} // namespace icpl

#endif // UTILS_H
