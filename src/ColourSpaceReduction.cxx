#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<opencv2/opencv.hpp>

cv::Mat& reduceColoursSingleChannel(cv::Mat& mat, const uchar* const lookupTable){
    std::for_each(mat.begin<uchar>(), mat.end<uchar>(), [&lookupTable](auto& cell){
        cell = lookupTable[cell];
    });
    return mat;
}

cv::Mat& reduceColoursTripleChannel(cv::Mat& mat, const uchar* const lookupTable){
    std::for_each(mat.begin<cv::Vec3b>(), mat.end<cv::Vec3b>(), [&lookupTable](auto& cell){
        cell[0] = lookupTable[cell[0]];
        cell[1] = lookupTable[cell[1]];
        cell[2] = lookupTable[cell[2]];
    });
    return mat;
}

cv::Mat& reduceColours(cv::Mat& mat, int divider){
    const int channels = mat.channels();
    const int grayScale = 1;
    const int colourScale = 3;

    uchar lookupTable[256];
    for(int i = 0; i < 256; ++i){
        lookupTable[i] = (uchar)(divider * (i/divider));
    }

    if(channels == grayScale){
        reduceColoursSingleChannel(mat, lookupTable);
    }
    else if(channels == colourScale){
        reduceColoursTripleChannel(mat, lookupTable);
    }
    return mat;
}
