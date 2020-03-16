#include<iostream>
#include<algorithm>
#include<array>
#include<opencv2/opencv.hpp>
#include"POBR/Preprocessing.hxx"

namespace POBR {

cv::Mat& HistogramEqualizer::equalize(cv::Mat& image){
    const std::array<int, 256> histogram = this->createHistogram(image);
    int pixelCount = image.rows * image.cols;
    const std::array<int, 256> lut = this->createLookuptable(histogram, pixelCount);
    return this->applyLUT(image, lut);
}  

const std::array<int, 256> HistogramEqualizer::createHistogram(cv::Mat& image) const {
    std::array<int, 256> histogram;
    std::for_each(histogram.begin(), histogram.end(), [](int& value){
        value = 0;
    });

    image.forEach<cv::Vec3b>([&histogram](cv::Vec3b& pixel, const int position[]){
        const uchar value = pixel[2];
        ++histogram[value];
    });

    return histogram;
}

const std::array<int, 256> HistogramEqualizer::createLookuptable(const std::array<int, 256> histogram, const int pixelCount) const {
    std::array<int, 256> lookupTable;
    int probabilitySum = 0;
    std::transform(histogram.begin(), histogram.end(), lookupTable.begin(), [&probabilitySum, &pixelCount](int bucket) -> int {
        probabilitySum += bucket;
        return probabilitySum * 255 / pixelCount;
    });
    
    return lookupTable;
}

cv::Mat& HistogramEqualizer::applyLUT(cv::Mat& image, const std::array<int, 256> lut) const {
    image.forEach<cv::Vec3b>([&lut](cv::Vec3b& pixel, const int position[]){
        pixel[2] = lut[pixel[2]];
    });
}

};