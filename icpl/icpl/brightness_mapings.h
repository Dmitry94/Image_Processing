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
 * Make correction with reference color.
 * Formula:
 *  ch = ch * src / dst
 *
 * @param source[in]    Source image.
 * @param src_color[in] Source color.
 * @param dst_color[in] Destination color.
 *
 * @return Corrected image.
 */
cv::Mat correct_with_colors(const cv::Mat &source,
                            const cv::Scalar &src_color,
                            const cv::Scalar &dst_color);

} // namespace icpl

#endif // BRIGHTNESS_MAPINGS_H

