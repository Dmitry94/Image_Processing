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

cv::Mat draw_histogram(const std::vector<std::vector<float>> &histograms,
                       const std::vector<cv::Scalar> &colors) {
    const size_t channels = histograms.size();
    if (channels != colors.size()) {
        throw std::length_error("draw_histogram: Histograms count should be "
                                "equal to colors count!");
    }

    for (size_t i = 0; i < channels; i++) {
        if (histograms[i].size() != 256) {
            throw std::length_error("draw_histogram: histogram should have"
                                         " size equal to 256!");
        }
    }

    int hist_w = 512; int hist_h = 300;
    int bin_w = cvRound((double)hist_w / 256);
    cv::Mat histogram_image(hist_h, hist_w, CV_8UC3, cv::Scalar(0));

    for (int j = 0; j < 256; j++) {
        for (size_t i = 0; i < channels; i++) {
            cv::line(histogram_image,
                     cv::Point(bin_w*(j-1), hist_h - cvRound(hist_h * histograms[i][j-1])),
                     cv::Point(bin_w*(j), hist_h - cvRound(hist_h * histograms[i][j])),
                     colors[i], 2, 8, 0);
        }
    }

    return histogram_image;
}

} // namespace icpl
