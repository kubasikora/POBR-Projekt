#include<opencv2/opencv.hpp>
#include<algorithm>
#include<array>
#include<experimental/array>
#include"POBR/Filters.hxx"

namespace POBR {

ConvolutionalFilter::ConvolutionalFilter(const cv::Mat& kernel) 
    : kernel_(kernel), filterOffsetX_(kernel.cols / 2), filterOffsetY_(kernel.rows / 2) {}


cv::Mat ConvolutionalFilter::filter(cv::Mat& image){
    const int imageHeight = image.rows, imageWidth = image.cols;
    cv::Mat filteredImage(cv::Size(imageWidth, imageHeight), image.type()); // size(width, height)

    filteredImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int y = position[0], x = position[1];
        if(x < filterOffsetX_ || y < filterOffsetY_ || x >= imageWidth - filterOffsetX_ || y >= imageHeight - filterOffsetY_){
            pixel = image.at<cv::Vec3b>(y, x);
            return;
        }

        std::array<double, 3> temporaryPixel = std::experimental::make_array(0.0, 0.0, 0.0);
        for(auto i = -filterOffsetX_; i <= filterOffsetX_; ++i)
            for(auto j = -filterOffsetY_; j <= filterOffsetY_; ++j)
                for(auto k = 0; k < 3; ++k)
                    temporaryPixel[k] += kernel_.at<double>(filterOffsetY_ + j, filterOffsetX_ + i) * image.at<cv::Vec3b>(y-j, x-i)[k];
    
        for(auto k = 0; k < 3; ++k){
            auto x = normPixel(temporaryPixel[k]);
            pixel[k] = x;
        }
    });
    return filteredImage;
}

uchar ConvolutionalFilter::normPixel(double i){
    if(i < 0) return 0;
    if(i > 255) return 255;
    return i;
}

}
