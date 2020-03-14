#include<opencv2/opencv.hpp>
#include"POBR/Masks.hxx"
#include<iostream>
#include<exception>

namespace POBR {



ChannelInterval::ChannelInterval(const int lower, const int upper) : 
                                lower_(lower), 
                                upper_(upper) {
    if(lower >= upper)
        throw std::exception();                           
}

ChannelInterval::~ChannelInterval() {}

const int ChannelInterval::getLowerBoundary() const {
    return lower_;
}

const int ChannelInterval::getUpperBoundary() const {
    return upper_;
}

const bool ChannelInterval::isInRange(const uchar pixel) const {
    return pixel >= this->getLowerBoundary() && pixel <= this->getUpperBoundary();
}

HueInterval::HueInterval(const int lower, const int upper) : 
                        ChannelInterval(lower, upper) {}

HueInterval::~HueInterval(){}

const bool HueInterval::isInRange(const uchar pixel) const {
    const int normalizedPixel = static_cast<int>(pixel) * 2;
    const int angles = 360;
    if(lower_ >= 0 && upper_ >= 0){
        if(lower_ <= upper_){
            return lower_ <= normalizedPixel && normalizedPixel <= upper_;
        } 
        return false;
    }
    if(lower_ < 0 && upper_ >= 0){
        const int newLower = angles + lower_;
        return normalizedPixel >= newLower || normalizedPixel <= upper_;
    }
    if(lower_ < 0 && upper_ < 0){
        if(upper_ < lower_) return false;
        const int newLower = angles + lower_;
        const int newUpper = angles + upper_;
        return newLower <= normalizedPixel && normalizedPixel <= newUpper;  
    }
    if(lower_ >= 0 && upper_ < 0){
        const int newUpper = angles + upper_;
        return lower_ <= normalizedPixel && normalizedPixel <= newUpper;
    }    
    return false;
} 

SaturationInterval::SaturationInterval(const int lower, const int upper) : 
    ChannelInterval(lower, upper) {}

SaturationInterval::~SaturationInterval(){}

ValueInterval::~ValueInterval(){}

ValueInterval::ValueInterval(const int lower, const int upper) : 
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
    if(!interval.isInRange(pixel[position])) {
        pixel[0] = 0;
        pixel[1] = 0;
        pixel[2] = 0;
    }   
    return pixel;    
}

};