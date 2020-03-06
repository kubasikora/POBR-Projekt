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

const uchar ChannelInterval::getLowerBoundary() const {
    return lower_;
}

const uchar ChannelInterval::getUpperBoundary() const {
    return upper_;
}

HSVMask::HSVMask(const ChannelInterval hue, const ChannelInterval saturation, const ChannelInterval value) :
                    hueInterval_(hue), saturationInterval_(saturation), valueInterval_(value) {}

cv::Mat& HSVMask::apply(cv::Mat& image){
    image.forEach<cv::Vec3b>([this](cv::Vec3b& pixel, const int position[]){
        pixel = this->maskPixel(pixel, 0, this->hueInterval_);
        pixel = this->maskPixel(pixel, 1, this->saturationInterval_);
        pixel = this->maskPixel(pixel, 2, this->valueInterval_);
    });
    return image;
}

cv::Vec3b& HSVMask::maskPixel(cv::Vec3b& pixel, const int position, const ChannelInterval interval){
    const bool condition = pixel[position] >= interval.getLowerBoundary() && pixel[position] < interval.getUpperBoundary();
    if(!condition) {
        pixel[0] = 0;
        pixel[1] = 0;
        pixel[2] = 0;
    }   
    return pixel;    
}

};