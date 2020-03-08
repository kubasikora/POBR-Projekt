#include<iostream>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include"POBR/Preprocessing.hxx"

namespace POBR {

cv::Mat& BGR2HSVConverter::convert(cv::Mat& image){
    image.forEach<cv::Vec3b>(BGR2HSVPixelConverter());
}

void BGR2HSVConverter::BGR2HSVPixelConverter::operator()(cv::Vec3b& pixel, const int position[]) const {
    const uchar red = pixel[2], green = pixel[1], blue = pixel[0];
    const uchar value = evalValue(red, green, blue);
    const uchar saturation = evalSaturation(red, green, blue, value);
    const uchar hue = evalHue(red, green, blue, value);
    pixel = {hue, saturation, value};
}

uchar BGR2HSVConverter::BGR2HSVPixelConverter::evalValue(const uchar red, const uchar green, const uchar blue) const {
    return std::max(std::max(red, green), blue);
}

uchar BGR2HSVConverter::BGR2HSVPixelConverter::evalSaturation(const uchar red, const uchar green, const uchar blue, const uchar value) const {
    uchar saturation = 0;
    const uchar minimum = std::min(std::min(red,  green), blue);
    if(value)
        saturation = (value - minimum) * 255 / value;
    return saturation;
}

uchar BGR2HSVConverter::BGR2HSVPixelConverter::evalHue(const uchar red, const uchar green, const uchar blue, const uchar value) const {
    int hue = 0;
    const int minimum = std::min(std::min(red, green), blue);
    if(minimum == value)
        return hue;

    const uchar divider = value - minimum;
    if(red == value)
        hue = ((green - blue) * 60 / divider) + 0;
        
    if(green == value)
        hue = ((blue - red) * 60 / divider) + 120;
        
    if(blue == value)
        hue = ((red - green) * 60 / divider) + 240;

    if(hue < 0)
        hue += 360;

    return static_cast<uchar>(hue / 2);    
}

cv::Mat& HSV2BGRConverter::convert(cv::Mat& image){
    image.forEach<cv::Vec3b>(HSV2BGRPixelConverter());
}

void HSV2BGRConverter::HSV2BGRPixelConverter::operator()(cv::Vec3b& pixel, const int position[]) const {  
    const uchar hue = pixel[0], saturation = pixel[1], value = pixel[2];
    const float h = hue * 2.0f;
    const float s = saturation / 255.0f;
    const float v = value / 255.0f;

    const float c = v * s;
    const float part = h / 60.0f;
    const float x = c * (1 - std::fabs(std::fmod(part,2) - 1));
    const float m = v - c;

    float r,g,b;

    switch(static_cast<int>(std::floor(part))){
      case 0:
        r = c; g = x; b = 0.0f; break;
      case 1:
        r = x; g = c; b = 0.0f; break;
      case 2:
        r = 0.0f; g = c; b = x; break;
      case 3:
        r = 0.0f; g = x; b = c; break;
      case 4:
        r = x; g = 0.0f; b = c; break;
      case 5:
        r = c; g = 0.0f; b = x; break;
      default: 
        r = c; g = 0.0f; b = x; break;
    }
    
    pixel[0] = static_cast<uchar>((b+m)*255);
    pixel[1] = static_cast<uchar>((g+m)*255);
    pixel[2] = static_cast<uchar>((r+m)*255);
}

};
