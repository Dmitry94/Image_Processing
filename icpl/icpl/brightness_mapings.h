#ifndef BRIGHTNESS_MAPINGS_H
#define BRIGHTNESS_MAPINGS_H

#include <vector>
#include <functional>

#include <opencv2/core/core.hpp>

namespace icpl {

/**
 * Builds look-up-table for brightness values.
 *
 * @param func[in]  Function for mapping.
 *
 * @return look-up-table.
 */
std::vector<uchar> build_LUT(const std::function<uchar(uchar)> &func);


/**
 * Apply LUT to the image and return result.
 *
 * @param source[in]    Source image.
 * @param LUTs[in]      LUTs array for each channel.
 *
 * @return Mapped image.
 */
cv::Mat apply_LUTs(const cv::Mat &source,
                   const std::vector<std::vector<uchar>> &LUTs);


/**
 * Make correction with reference colors.
 * Formula:
 *  ch = ch * src / dst
 *
 * @param source[in]    Source image.
 * @param src_color[in] Source color.
 * @param dst_color[in] Destination color.
 *
 * @return Corrected image.
 */
cv::Mat correct_with_reference_colors(const cv::Mat &source,
                                      const cv::Scalar &src_color,
                                      const cv::Scalar &dst_color);


/**
 * Apply gray world effect to the image.
 * Formula:
 *  ch = ch * avg_all / arg_ch
 *
 * @param source[in]    Source image.
 *
 * @return Gray world image.
 */
cv::Mat apply_gray_world_effect(const cv::Mat &source);


/**
 * Apply gamma correction to the image.
 *
 * @param source[in]    Source image.
 * @param power[in]     Power of the effect, [0..100] range
 *
 * @return Result of gamma correction.
 */
cv::Mat apply_gamma_correction(const cv::Mat &source, const int power);


/**
 * Apply contrast correction to the image.
 *
 * @param source[in]    Source image.
 * @param power[in]     Power of the effect, [0..100] range
 *
 * @return Result of contrast correction.
 */
cv::Mat apply_contrast_correction(const cv::Mat &source, const int power);


} // namespace icpl

#endif // BRIGHTNESS_MAPINGS_H

