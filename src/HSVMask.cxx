#include<opencv2/opencv.hpp>
#include"POBR/Masks.hxx"
#include<iostream>
#include<exception>

namespace POBR {



ChannelInterval::ChannelInterval(const uchar lower, const uchar upper) : 
                                lower_(lower), 
                                upper_(upper) {
    if(lower >= upper)
        throw std::exception();                           
}

ChannelInterval::~ChannelInterval() {}

const uchar ChannelInterval::getLowerBoundary() const {
    return lower_;
}

const uchar ChannelInterval::getUpperBoundary() const {
    return upper_;
}

HueInterval::HueInterval(const int lower, const int upper) : 
    ChannelInterval(static_cast<uchar>(lower/2), static_cast<uchar>(upper/2)) {}

HueInterval::~HueInterval(){}

SaturationInterval::SaturationInterval(const uchar lower, const uchar upper) : 
    ChannelInterval(lower, upper) {}

SaturationInterval::~SaturationInterval(){}

ValueInterval::~ValueInterval(){}

ValueInterval::ValueInterval(const uchar lower, const uchar upper) : 
    ChannelInterval(lower, upper) {}

HSVMask::HSVMask(const HueInterval hue, const SaturationInterval saturation, const ValueInterval value) :
                    hueInterval_(hue), saturationInterval_(saturation), valueInterval_(value) {}

cv::Mat& HSVMask::apply(cv::Mat& image){
    image.forEach<cv::Vec3b>([this](cv::Vec3b& pixel, const int position[]){
        pixel = this->maskPixel(pixel, 0, this->hueInterval_);
        pixel = this->maskPixel(pixel, 1, this->saturationInterval_);
        pixel = this->maskPixel(pixel, 2, this->valueInterval_);
    });
    return image;
}

cv::Vec3b& HSVMask::maskPixel(cv::Vec3b& pixel, const int position, const ChannelInterval& interval){
    const bool condition = pixel[position] >= interval.getLowerBoundary() && pixel[position] <= interval.getUpperBoundary();
    if(!condition) {
        pixel[0] = 0;
        pixel[1] = 0;
        pixel[2] = 0;
    }   
    return pixel;    
}

};