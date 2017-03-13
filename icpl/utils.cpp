#include <vector>
#include <stdexcept>

#include <opencv2/opencv.hpp>

#include "icpl/utils.h"

namespace icpl {

std::vector<std::vector<float>> build_histograms(const cv::Mat &src) {
    int channels = src.channels();
    int rows = src.rows;
    int cols = src.cols;

    if (src.step[1] / channels != sizeof(uchar)) {
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

cv::Mat draw_histogram(const std::vector<float> &histogram) {
    const int hist_size = histogram.size();
    if (hist_size != 256) {
        throw std::length_error("draw_histogram: histogram should have"
                                     " size equal to 256!");
    }

    const int hist_w = 256 * 7;
    const int hist_h = 256 * 1;
    cv::Mat histogram_image(hist_h, hist_w, CV_8UC1, cv::Scalar(0));

    for(int i = 0; i < hist_size; i++) {
        int cur_height = hist_h - cvRound(hist_h * histogram[i]);

        for (int y = cur_height; y < hist_h; y++) {
            uchar *cur_line = histogram_image.ptr(y) + i * hist_w / hist_size;

            for (int k = 0; k < hist_w / hist_size; k++) {
                    cur_line[0] = 255;
                    cur_line++;
            }
        }
    }

    return histogram_image;
}

} // namespace icpl
