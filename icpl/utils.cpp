#include <vector>
#include <stdexcept>

#include <opencv2/core/core.hpp>

#include "utils.h"

namespace icpl {

std::vector<std::vector<float>> build_histogram(const cv::Mat &src) {
    int channels = src.channels();
    int rows = src.rows;
    int cols = src.cols;

    if (src.type() != CV_8U) {
        throw std::logic_error("build_histogram: src image should have "
                               "uchar type!");
    }

    std::vector<std::vector<float>> histogram(channels);
    for (int i = 0; i < channels; i++) {
        histogram[i].resize(256);
    }

    if (src.isContinuous()) {
        cols = rows * cols;
        rows = 1;
    }

#pragma omp parallel for if(rows > 1)
    for (int i = 0; i < rows; i++) {
        const uchar* cur_line = src.ptr(i);

#pragma omp parallel for if(rows == 1)
        for (int j = 0; j < cols; j++) {
            for (int ch = 0; ch < channels; ch++) {
                const uchar cur_color = cur_line[(j - 1) * channels + ch];
                histogram[ch][cur_color]++;
            }
        }
    }

#pragma omp parallel for
    for (int i = 0; i < channels; i++) {
        for (int j = 0; j < 256; j++) {
            histogram[i][j] = histogram[i][j] / (rows * cols);
        }
    }

    return histogram;
}

}
