#include"POBR/Identification.hxx"

namespace POBR {

cv::Mat drawSegmentBoundary(cv::Mat& image, BoundingBox bb){
    const cv::Vec3b lime = {50, 205, 50};

    for(auto i = bb.x; i < bb.x + bb.width; ++i){
        image.at<cv::Vec3b>(bb.y, i) = lime;
    }

    for(auto i = bb.y; i < bb.y + bb.height; ++i){
        image.at<cv::Vec3b>(i, bb.x) = lime;
    }

    for(auto i = bb.x; i < bb.x + bb.width; ++i){
        image.at<cv::Vec3b>(bb.y + bb.height, i) = lime;
    }

    for(auto i = bb.y; i < bb.y + bb.height; ++i){
        image.at<cv::Vec3b>(i, bb.x + bb.width) = lime;
    }

    return image;
}

};