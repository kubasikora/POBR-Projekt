#include<iostream>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include"POBR/Functors.hxx"

namespace POBR {

void BGR2HSVPixelConverter::operator()(cv::Vec3b& pixel, const int position[]) const {
    const uchar red = pixel[2], green = pixel[1], blue = pixel[0];
    const uchar value = evalValue(red, green, blue);
    const uchar saturation = evalSaturation(red, green, blue, value);
    const uchar hue = evalHue(red, green, blue, value);
    pixel = {hue, saturation, value};
}

uchar BGR2HSVPixelConverter::evalValue(const uchar red, const uchar green, const uchar blue) const {
    return std::max(std::max(red, green), blue);
}

uchar BGR2HSVPixelConverter::evalSaturation(const uchar red, const uchar green, const uchar blue, const uchar value) const {
    uchar saturation = 0;
    const uchar minimum = std::min(std::min(red,  green), blue);
    if(value)
        saturation = (value - minimum) * 255 / value;
    return saturation;
}

uchar BGR2HSVPixelConverter::evalHue(const uchar red, const uchar green, const uchar blue, const uchar value) const {
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

};
