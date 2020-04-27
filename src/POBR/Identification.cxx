#include"POBR/Identification.hxx"

namespace POBR {

cv::Mat drawSegmentBoundary(cv::Mat& image, BoundingBox bb){
    const cv::Vec3b line = {255, 0, 255};

    for(auto i = bb.x; i < bb.x + bb.width; ++i){
        image.at<cv::Vec3b>(bb.y, i) = line;
        image.at<cv::Vec3b>(bb.y, i+1) = line;
        image.at<cv::Vec3b>(bb.y, i-1) = line;
        image.at<cv::Vec3b>(bb.y+1, i) = line;
        image.at<cv::Vec3b>(bb.y+1, i+1) = line;
        image.at<cv::Vec3b>(bb.y+1, i-1) = line;
        image.at<cv::Vec3b>(bb.y-1, i) = line;
        image.at<cv::Vec3b>(bb.y-1, i+1) = line;
        image.at<cv::Vec3b>(bb.y-1, i-1) = line;
    }

    for(auto i = bb.y; i < bb.y + bb.height; ++i){
        image.at<cv::Vec3b>(i, bb.x+1) = line;
        image.at<cv::Vec3b>(i, bb.x-1) = line;
        image.at<cv::Vec3b>(i, bb.x) = line;
        image.at<cv::Vec3b>(i+1, bb.x+1) = line;
        image.at<cv::Vec3b>(i+1, bb.x-1) = line;
        image.at<cv::Vec3b>(i+1, bb.x) = line;
        image.at<cv::Vec3b>(i-1, bb.x+1) = line;
        image.at<cv::Vec3b>(i-1, bb.x-1) = line;
        image.at<cv::Vec3b>(i-1, bb.x) = line;
    }

    for(auto i = bb.x; i < bb.x + bb.width; ++i){
        image.at<cv::Vec3b>(bb.y + bb.height, i) = line;
        image.at<cv::Vec3b>(bb.y + bb.height, i+1) = line;
        image.at<cv::Vec3b>(bb.y + bb.height, i-1) = line;
        image.at<cv::Vec3b>(bb.y +1  + bb.height, i) = line;
        image.at<cv::Vec3b>(bb.y +1  + bb.height, i+1) = line;
        image.at<cv::Vec3b>(bb.y +1 + bb.height, i-1) = line;
        image.at<cv::Vec3b>(bb.y -1 + bb.height, i) = line;
        image.at<cv::Vec3b>(bb.y -1 + bb.height, i+1) = line;
        image.at<cv::Vec3b>(bb.y -1 + bb.height, i-1) = line;
    }

    for(auto i = bb.y; i < bb.y + bb.height; ++i){
        image.at<cv::Vec3b>(i, bb.x + bb.width + 1) = line;
        image.at<cv::Vec3b>(i, bb.x + bb.width - 1) = line;
        image.at<cv::Vec3b>(i, bb.x + bb.width) = line;
        image.at<cv::Vec3b>(i + 1, bb.x + bb.width + 1) = line;
        image.at<cv::Vec3b>(i + 1, bb.x + bb.width - 1) = line;
        image.at<cv::Vec3b>(i + 1, bb.x + bb.width) = line;
        image.at<cv::Vec3b>(i - 1, bb.x + bb.width + 1) = line;
        image.at<cv::Vec3b>(i - 1, bb.x + bb.width - 1) = line;
        image.at<cv::Vec3b>(i - 1, bb.x + bb.width) = line;
    }

    return image;
}

};