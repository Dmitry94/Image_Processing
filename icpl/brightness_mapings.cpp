#include <vector>
#include <opencv2/opencv.hpp>

#include <icpl/brightness_mapings.h>

namespace icpl {

std::vector<uchar> build_LUT(const std::function<uchar(uchar)> &func) {
    std::vector<uchar> LUT(256);

    for (auto i = 0; i < 256; i++) {
        LUT[i] = func(i);
    }

    return LUT;
}

cv::Mat correct_with_reference_colors(const cv::Mat &source,
                                      const cv::Scalar &src_color,
                                      const cv::Scalar &dst_color) {
    if (source.channels() > 4) {
        throw std::logic_error("correct_with_reference_colors: too much channels!");
    }

    std::vector<std::vector<uchar>> LUTs(source.channels());

#pragma omp parallel for
    for (int i = 0; i < source.channels(); i++) {
        float cur_ch_koef = dst_color[i] / (float)src_color[i];
        LUTs[i] = build_LUT([&cur_ch_koef](uchar bright)
                                { return bright * cur_ch_koef; }
                            );
    }

    cv::Mat result = source.clone();
    int rows = result.rows;
    int cols = result.cols;
    int channels = result.channels();
    if (result.isContinuous()) {
        cols = rows * cols;
        rows = 1;
    }

#pragma omp parallel for if(rows > 1)
    for (int i = 0; i < rows; i++) {
        uchar *cur_line = result.ptr(i);

#pragma omp parallel for if(rows == 1)
        for (int j = 0; j < cols; j++) {
            for (int ch = 0; ch < result.channels(); ch++) {
                int cur_index = j * channels + ch;
                uchar cur_value = cur_line[cur_index];
                cur_line[cur_index] = LUTs[ch][cur_value];
            }
        }
    }

    return result;
}

} //namespace icpl
