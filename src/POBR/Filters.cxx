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

MedianFilter::MedianFilter(const int windowSize) : 
    windowSize_(windowSize % 2 == 1 ? windowSize_ : throw InvalidWindowSizeException()),
    offset_(windowSize / 2) {}

cv::Mat MedianFilter::filter(cv::Mat& image){
    const int imageHeight = image.rows, imageWidth = image.cols;
    std::cout << imageHeight << " : " << imageWidth << std::endl;
    cv::Mat filteredImage(imageWidth, imageHeight, image.type()); // size(width, height)
    std::cout << "image created" << std::endl;

    filteredImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int y = position[0], x = position[1];
        if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
            pixel = image.at<cv::Vec3b>(y, x);
            return;
        }

        std::array<std::vector<uchar>,3> pixels;
        std::for_each(pixels.begin(), pixels.end(), [&](auto& channel){
            channel.reserve(windowSize_^2);
        });
       
        for(auto i = -offset_; i <= offset_; ++i)
            for(auto j = -offset_; j <= offset_; ++j){
                for(auto k = 0; k < 3; ++k){
                    pixels[k].push_back(image.at<cv::Vec3b>(y-j, x-i)[k]);
                }
        }
        
        int counter = 0;
        std::for_each(pixels.begin(), pixels.end(), [&](auto& channel){
            std::sort(channel.begin(), channel.end());
            pixel[counter] = channel[windowSize_^2 / 2];
            ++counter;
        });
    });
    return filteredImage;
}

ErosionFilter::ErosionFilter(const int windowSize) : 
    windowSize_(windowSize % 2 == 1 ? windowSize_ : throw InvalidWindowSizeException()),
    offset_(windowSize / 2) {}

cv::Mat ErosionFilter::filter(cv::Mat& image){
    const int imageHeight = image.rows, imageWidth = image.cols;
    cv::Mat filteredImage(cv::Size(imageWidth, imageHeight), image.type()); // size(width, height)

    filteredImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int y = position[0], x = position[1];
        if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
            pixel = image.at<cv::Vec3b>(y, x);
            return;
        }

        std::array<std::vector<uchar>,3> pixels;
        std::for_each(pixels.begin(), pixels.end(), [&](auto& channel){
            channel.reserve(windowSize_^2);
        });
       
        for(auto i = -offset_; i <= offset_; ++i)
            for(auto j = -offset_; j <= offset_; ++j){
                for(auto k = 0; k < 3; ++k){
                    pixels[k].push_back(image.at<cv::Vec3b>(y-j, x-i)[k]);
                }
        }
        
        int counter = 0;
        std::for_each(pixels.begin(), pixels.end(), [&](auto& channel){
            std::sort(channel.begin(), channel.end());
            pixel[counter] = channel.front();
            ++counter;
        });
    });

    return filteredImage;
}

DilationFilter::DilationFilter(const int windowSize) : 
    windowSize_(windowSize % 2 == 1 ? windowSize_ : throw InvalidWindowSizeException()),
    offset_(windowSize / 2) {}

cv::Mat DilationFilter::filter(cv::Mat& image){
    const int imageHeight = image.rows, imageWidth = image.cols;
    cv::Mat filteredImage(cv::Size(imageWidth, imageHeight), image.type()); // size(width, height)

    filteredImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int y = position[0], x = position[1];
        if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
            pixel = image.at<cv::Vec3b>(y, x);
            return;
        }

        std::array<std::vector<uchar>,3> pixels;
        std::for_each(pixels.begin(), pixels.end(), [&](auto& channel){
            channel.reserve(windowSize_^2);
        });
       
        for(auto i = -offset_; i <= offset_; ++i)
            for(auto j = -offset_; j <= offset_; ++j){
                for(auto k = 0; k < 3; ++k){
                    pixels[k].push_back(image.at<cv::Vec3b>(y-j, x-i)[k]);
                }
        }
        
        int counter = 0;
        std::for_each(pixels.begin(), pixels.end(), [&](auto& channel){
            std::sort(channel.begin(), channel.end());
            pixel[counter] = channel.back();
            ++counter;
        });
    });

    return filteredImage;
}

GaussianFilter::GaussianFilter(const int windowSizeX, const int windowSizeY, const double variance) : 
    // filterOffsetX_(windowSizeX % 2 == 1 ? windowSizeX / 2 : throw InvalidWindowSizeException()), 
    // filterOffsetY_(windowSizeX % 2 == 1 ? windowSizeX / 2 : throw InvalidWindowSizeException()), 
    // kernel_(createKernel(windowSizeX, windowSizeY, variance)) {}
    ConvolutionalFilter(createKernel(windowSizeX, windowSizeY, variance)) {}

cv::Mat GaussianFilter::createKernel(const int windowSizeX, const int windowSizeY, const double variance){
    cv::Mat_<double> kernel(cv::Size(windowSizeX, windowSizeY));
    const int xOffset = windowSizeX / 2;
    const int yOffset = windowSizeY / 2;
    const double pi = 3.1415;
    kernel.forEach([&](double& cell, const int position[]){
        const int y = position[0], x = position[1];
        cell = 1 / (2*pi*std::pow(variance,2)) * std::exp(-(std::pow(x - xOffset, 2) + std::pow(y - yOffset, 2))/(2*std::pow(variance, 2)));
    });
    return kernel;
}

// cv::Mat GaussianFilter::filter(cv::Mat& image){
//     return image;
// }


}
