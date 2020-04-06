#include<opencv2/opencv.hpp>
#include"POBR/Masks.hxx"
#include<iostream>
#include<exception>
#include<numeric>

namespace POBR {

ChannelInterval::ChannelInterval(const int lower, const int upper) : 
                                lower_(lower), 
                                upper_(upper) {
    if(lower >= upper)
        throw std::exception();                           
}

ChannelInterval::~ChannelInterval() {}

int ChannelInterval::getLowerBoundary() const {
    return lower_;
}

int ChannelInterval::getUpperBoundary() const {
    return upper_;
}

bool ChannelInterval::isInRange(const uchar pixel) const {
    return pixel >= this->getLowerBoundary() && pixel <= this->getUpperBoundary();
}

HueInterval::HueInterval(const int lower, const int upper) : 
                        ChannelInterval(lower, upper) {}

HueInterval::~HueInterval(){}

bool HueInterval::isInRange(const uchar pixel) const {
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

cv::Mat HSVMask::apply(cv::Mat& image){
    cv::Mat maskedImage(image.size(), CV_8U);
    image.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        if(this->hueInterval_.isInRange(pixel[0]) && this->saturationInterval_.isInRange(pixel[1]) && this->valueInterval_.isInRange(pixel[2])){
            maskedImage.at<uchar>(position) = 255;
        } else {
            maskedImage.at<uchar>(position) = 0;
        }
    });
    return maskedImage;
}

cv::Vec3b& HSVMask::maskPixel(cv::Vec3b& pixel, const int position, const ChannelInterval& interval){
    if(!interval.isInRange(pixel[position])) {
        pixel[0] = 0;
        pixel[1] = 0;
        pixel[2] = 0;
    }   
    return pixel;    
}

MaskApplier& MaskApplier::add(HSVMask mask){
    masks_.push_back(mask);
    return *this;
}

cv::Mat& MaskApplier::apply(cv::Mat& image){
    if(masks_.empty())
        return image;

    std::vector<cv::Mat> partials;
    std::for_each(masks_.begin(), masks_.end(), [&partials, &image](HSVMask& mask){
        cv::Mat partialImage = image.clone();
        partials.push_back(mask.apply(partialImage));
    });

    auto imageFolder = [](cv::Mat& x, cv::Mat& y){
        return x | y;
    };

    image = std::accumulate(partials.begin(), partials.end(), cv::Mat(image.rows, image.cols, image.type()), imageFolder);
    return image;
}

HSVMask MaskFactory::build(){
    const HueInterval h(config_.at("lowerHue"), config_.at("upperHue"));
    const SaturationInterval s(config_.at("lowerSaturation"), config_.at("upperSaturation"));
    const ValueInterval v(config_.at("lowerValue"), config_.at("upperValue"));
    return HSVMask(h, s, v);
}

};