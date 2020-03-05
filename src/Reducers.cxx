#include<iostream>
#include<string>
#include<opencv2/opencv.hpp>
#include"POBR/Preprocessing.hxx"

namespace POBR {

cv::Mat& ColorReducer::reduce(cv::Mat& image, const int divider){
    const int channels = image.channels();
    const std::array<uchar, 256> lookupTable = createLookupTable(divider);

    if(channels == GRAYSCALE){
        reduceSingleChannel(image, lookupTable);
    } else if (channels == COLORSCALE){
        reduceTripleChannel(image, lookupTable);
    }

    return image;
}

const std::array<uchar, 256> ColorReducer::createLookupTable(const int divider){
    std::array<uchar, 256> table;
    for(int i = 0; i < 256; ++i){
        table[i] = (uchar)(divider * (i / divider));
    }
    return table;
}

cv::Mat& ColorReducer::reduceSingleChannel(cv::Mat& image, const std::array<uchar, 256> lookupTable){
    image.forEach<uchar>([&lookupTable](uchar pixel, const int position[]){
        pixel = lookupTable[pixel];
    });
    return image;
}

cv::Mat& ColorReducer::reduceTripleChannel(cv::Mat& image, const std::array<uchar, 256> lookupTable){
    image.forEach<cv::Vec3b>([&lookupTable](cv::Vec3b& pixel, const int position[]){
        pixel[0] = lookupTable[pixel[0]];
        pixel[1] = lookupTable[pixel[1]];
        pixel[2] = lookupTable[pixel[2]];
    });
    return image;
}   

};