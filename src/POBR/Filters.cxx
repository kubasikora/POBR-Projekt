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
    windowSize_(windowSize % 2 == 1 ? windowSize : throw InvalidWindowSizeException()),
    offset_(windowSize / 2) {}

cv::Mat MedianFilter::filter(cv::Mat& image){
    const int imageHeight = image.rows, imageWidth = image.cols;
    cv::Mat filteredImage(imageWidth, imageHeight, image.type()); // size(width, height)

    filteredImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int y = position[0], x = position[1];
        if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
            pixel = image.at<cv::Vec3b>(y, x);
            return;
        }
        
        for(auto k = 0; k < 3; ++k){
            std::vector<uchar> pixels;
            for(auto i = -offset_; i <= offset_; ++i){
                for(auto j = -offset_; j <= offset_; ++j){
                    pixels.push_back(image.at<cv::Vec3b>(y-j, x-i)[k]);
                }
            } 
            std::sort(pixels.begin(), pixels.end());
            pixel[k] = pixels[windowSize_^2 / 2];
        }
    });
    return filteredImage;
}

ErosionFilter::ErosionFilter(const int windowSize) : 
    windowSize_(windowSize % 2 == 1 ? windowSize : throw InvalidWindowSizeException()),
    offset_(windowSize / 2) {}

cv::Mat ErosionFilter::filter(cv::Mat& image){
    const int imageHeight = image.rows, imageWidth = image.cols;
    cv::Mat filteredImage(image.size(), image.type());

    if(image.type() == CV_8U){
        filteredImage.forEach<uchar>([&](uchar& pixel, const int position[]){
            const int y = position[0], x = position[1];
            if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
                pixel = image.at<uchar>(y, x);
                return;
            }
        
            for(auto i = -offset_; i <= offset_; ++i){
                for(auto j = -offset_; j <= offset_; ++j){
                    const uchar pixelTemporary = image.at<uchar>(y-j, x-i);
                    pixel = std::min(pixelTemporary, pixel);
                }
            }
        });
    } else {
        filteredImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
            const int y = position[0], x = position[1];
            if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
                pixel = image.at<cv::Vec3b>(y, x);
                return;
            }
        
            for(auto i = -offset_; i <= offset_; ++i){
                for(auto j = -offset_; j <= offset_; ++j){
                    for(auto k = 0; k < 3; ++k){
                        const uchar pixelTemporary = image.at<cv::Vec3b>(y-j, x-i)[k];
                        pixel[k] = std::min(pixelTemporary, pixel[k]);
                    }
                }
            }
        });
    }

    return filteredImage;
}

DilationFilter::DilationFilter(const int windowSize) : 
    windowSize_(windowSize % 2 == 1 ? windowSize : throw InvalidWindowSizeException()),
    offset_(windowSize / 2) {}

cv::Mat DilationFilter::filter(cv::Mat& image){
    const int imageHeight = image.rows, imageWidth = image.cols;
    cv::Mat filteredImage(image.size(), image.type()); // size(width, height)

    if(image.type() == CV_8U){
        std::cout << "lel" << std::endl;
        filteredImage.forEach<uchar>([&](uchar& pixel, const int position[]){
            const int y = position[0], x = position[1];
            if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
                pixel = image.at<uchar>(y, x);
                return;
            }
        
            for(auto i = -offset_; i <= offset_; ++i){
                for(auto j = -offset_; j <= offset_; ++j){
                    const uchar pixelTemporary = image.at<uchar>(y-j, x-i);
                    pixel = std::max(pixelTemporary, pixel);
                }
            }
        });
    }
    else {
        filteredImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
            const int y = position[0], x = position[1];
            if(x < offset_ || y < offset_ || x >= imageWidth - offset_ || y >= imageHeight - offset_){
                pixel = image.at<cv::Vec3b>(y, x);
                return;
            }
        
            for(auto i = -offset_; i <= offset_; ++i){
                for(auto j = -offset_; j <= offset_; ++j){
                    for(auto k = 0; k < 3; ++k){
                        const uchar pixelTemporary = image.at<cv::Vec3b>(y-j, x-i)[k];
                        pixel[k] = std::max(pixelTemporary, pixel[k]);
                    }
                }
            }
        });
    }

    return filteredImage;
}

GaussianFilter::GaussianFilter(const int windowSize, const double variance) : 
    ConvolutionalFilter(createKernel(windowSize, variance)) {}

cv::Mat GaussianFilter::createKernel(const int windowSize, const double variance){
    cv::Mat_<double> kernel(cv::Size(windowSize, windowSize));
    const int offset = windowSize / 2;
    const double pi = 3.1415;
    kernel.forEach([&](double& cell, const int position[]){
        const int y = position[0], x = position[1];
        cell = 1 / (2*pi*std::pow(variance,2)) * std::exp(-(std::pow(x - offset, 2) + std::pow(y - offset, 2))/(2*std::pow(variance, 2)));
    });
    return kernel;
}

}
