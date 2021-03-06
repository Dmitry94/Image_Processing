#include <vector>
#include <opencv2/opencv.hpp>

#include <icpl/brightness_mapings.h>
#include <icpl/utils.h>

namespace icpl {

std::vector<uchar> build_LUT(const std::function<uchar(uchar)> &func) {
    std::vector<uchar> LUT(256);

    for (auto i = 0; i < 256; i++) {
        LUT[i] = func(i);
    }

    return LUT;
}

cv::Mat apply_LUTs(const cv::Mat &source,
                   const std::vector<std::vector<uchar>> &LUTs) {
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
            for (int ch = 0; ch < channels; ch++) {
                int cur_index = j * channels + ch;
                uchar cur_value = cur_line[cur_index];
                cur_line[cur_index] = LUTs[ch][cur_value];
            }
        }
    }

    return result;
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
                    {
                        return cv::saturate_cast<uchar>(bright * cur_ch_koef);
                    });
    }

    cv::Mat result = apply_LUTs(source, LUTs);
    return result;
}


cv::Mat apply_gray_world_effect(const cv::Mat &source) {
    int rows = source.rows;
    int cols = source.cols;
    int channels = source.channels();
    if (source.isContinuous()) {
        cols = rows * cols;
        rows = 1;
    }

    std::vector<float> channels_avg(channels);
    float image_bright_avg = 0.0;
    #pragma omp parallel for if(rows > 1)
    for (int i = 0; i < rows; i++) {
        const uchar *cur_line = source.ptr(i);

        #pragma omp parallel for if(rows == 1)
        for (int j = 0; j < cols; j++) {
            for (int ch = 0; ch < channels; ch++) {
                channels_avg[ch] += cur_line[j * channels + ch];
            }
        }
    }

    for (size_t i = 0; i < channels_avg.size(); i++) {
        channels_avg[i] /= rows * cols;
        image_bright_avg += channels_avg[i];
    }
    image_bright_avg /= channels;


    std::vector<std::vector<uchar>> LUTs(source.channels());
    #pragma omp parallel for
    for (int i = 0; i < source.channels(); i++) {
        float cur_ch_avg = channels_avg[i];
        LUTs[i] = build_LUT([image_bright_avg, &cur_ch_avg](uchar bright)
                    {
                        return cv::saturate_cast<uchar>(bright * image_bright_avg
                                                     / cur_ch_avg);
                    });
    }

    cv::Mat result = apply_LUTs(source, LUTs);
    return result;
}


cv::Mat apply_gamma_correction(const cv::Mat &source, const int power) {
    if (power < 0 || power > 100) {
        throw std::out_of_range("apply_gamma_correction: range should be "
                                "in [0..100]");
    }
    int new_power = power - 50;

    std::vector<std::vector<uchar>> LUTs(source.channels());
    #pragma omp parallel for
    for (int i = 0; i < source.channels(); i++) {

        LUTs[i] = build_LUT([new_power](uchar bright)
                    {
                        float koef = std::pow(0.9, new_power);
                        float real_val = 255 * std::pow((float)bright / 255.0,
                                                        koef);
                        return cv::saturate_cast<uchar>(real_val);
                    });
    }

    cv::Mat result = apply_LUTs(source, LUTs);
    return result;
}


cv::Mat apply_contrast_correction(const cv::Mat &source, const int power) {
    if (power < 0 || power > 100) {
        throw std::out_of_range("apply_gamma_correction: range should be "
                                "in [0..100]");
    }
    int new_power = power - 50;

    std::vector<std::vector<uchar>> LUTs(source.channels());
    #pragma omp parallel for
    for (int i = 0; i < source.channels(); i++) {

        LUTs[i] = build_LUT([new_power](uchar bright)
                    {
                         int s1 = 50 - new_power;
                         int r1 = 50 + 87.5 * new_power / 50.0;

                         int s2 = 205 + new_power;
                         int r2 = 205 - 87.5 * new_power / 50.0;

                         if (bright <= r1) {
                             return cv::saturate_cast<uchar>(s1 * bright /
                                                             (r1 + 1));
                         }

                         if (bright <= r2) {
                             return cv::saturate_cast<uchar>((bright - r1) *
                                        (s2 - s1) / (r2 - r1 + 1) + s1);
                         }

                         return cv::saturate_cast<uchar>((bright - r2) *
                                                         (255 - s2) /
                                                         (255 - r2 + 1) + s2);
                    });
    }

    cv::Mat result = apply_LUTs(source, LUTs);
    return result;
}


cv::Mat apply_hist_normalization(const cv::Mat &source, const int k, const int b) {
    std::vector<std::vector<uchar>> LUTs(source.channels());
    #pragma omp parallel for
    for (int i = 0; i < source.channels(); i++) {

        LUTs[i] = build_LUT([k, b](uchar bright)
                    {
                        return cv::saturate_cast<uchar>(bright * k + b);
                    });
    }

    cv::Mat result = apply_LUTs(source, LUTs);
    return result;
}


cv::Mat apply_hist_equalization(const cv::Mat &source) {
    auto histograms = build_histograms(source);

    std::vector<std::vector<uchar>> LUTs(source.channels());
    #pragma omp parallel for
    for (int i = 0; i < source.channels(); i++) {
        auto cur_ch_hist = histograms[i];
        float sum = std::accumulate(cur_ch_hist.begin(),
                                    cur_ch_hist.end(), 0.0);

        LUTs[i] = build_LUT([&cur_ch_hist, &sum](uchar bright)
                    {
                        float sub_sum = std::accumulate(cur_ch_hist.begin(),
                                                        cur_ch_hist.begin() +
                                                        bright,
                                                        0.0);
                        return cv::saturate_cast<uchar>(255.0 * sub_sum / sum);
                    });
    }

    cv::Mat result = apply_LUTs(source, LUTs);
    return result;
}


} //namespace icpl
