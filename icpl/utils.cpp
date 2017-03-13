#include <vector>
#include <stdexcept>

#include <opencv2/opencv.hpp>

#include "utils.h"

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

//#pragma omp parallel for if(rows > 1)
    for (int i = 0; i < rows; i++) {
        const uchar* cur_line = src.ptr(i);

//#pragma omp parallel for if(rows == 1)
        for (int j = 0; j < cols; j++) {
            for (int ch = 0; ch < channels; ch++) {
                const uchar cur_color = cur_line[(j - 1) * channels + ch];
                histogram[ch][cur_color]++;
            }
        }
    }

//#pragma omp parallel for
    for (int i = 0; i < channels; i++) {
        for (int j = 0; j < 256; j++) {
            histogram[i][j] = histogram[i][j] / (rows * cols);
        }
    }

    return histogram;
}

void draw_histogram(const std::vector<float> &histogram,
                    const cv::Scalar &color, cv::Mat &histogram_image) {
    if (histogram_image.empty()) {
        throw std::logic_error("draw_histogram: image is empty!");
    }

    const int histSize = histogram.size();
    const int hist_w = histogram_image.cols;
    const int hist_h = histogram_image.rows;
    const int bin_w = cvRound((double) hist_w / histSize);

    for(int i = 1; i < histSize; i++) {
        cv::line(histogram_image, cv::Point(bin_w*(i-1),
                 hist_h - cvRound(hist_h * histogram[i-1])) ,
                 cv::Point(bin_w*(i), hist_h - cvRound(hist_h * histogram[i])),
                 color, 2, 8, 0);
    }
}

} // namespace icpl
