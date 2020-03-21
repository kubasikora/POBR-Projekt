#include<algorithm>
#include"POBR/Preprocessing.hxx"

namespace POBR {

cv::Mat BilinearInterpolationResizer::resize(cv::Mat& image){
    const double xRatio = static_cast<double>(image.rows) / static_cast<double>(xSize_), yRatio = static_cast<double>(image.cols) / static_cast<double>(ySize_);
    cv::Mat resizedImage(xSize_, ySize_, image.type());

    resizedImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int i = position[0], j = position[1];
        const double x = i*xRatio;
        const double y = j*yRatio;
        const int x0 = x, y0 = y; // floor 
        const double dx = x - x0, dy = y - y0; // frac
        
        int x1, y1;
        if (x0 + 1 > image.rows)
            x1 = x0;
        else
            x1 = x0 + 1;
        
        if (y0 + 1 > image.cols)
            y1 = y0;
        else
            y1 = y0 + 1;

        const cv::Vec3b p00 = image.at<cv::Vec3b>(x0, y0);
        const cv::Vec3b p01 = image.at<cv::Vec3b>(x0, y1);
        const cv::Vec3b p10 = image.at<cv::Vec3b>(x1, y0);
        const cv::Vec3b p11 = image.at<cv::Vec3b>(x1, y1);

        for(auto i = 0; i < pixel.channels; ++i){
            pixel[i] = norm(interpolate(interpolate(p00[i], p10[i], dx), interpolate(p01[i], p11[i], dx), dy));
        }
    });
    image = resizedImage;
    return image;
}

const double BilinearInterpolationResizer::interpolate(const double p1, const double p2, const double d){
    return p1*(1-d) + p2*d;
}

const uchar BilinearInterpolationResizer::norm(const double n){
    if(n > 255) return 255;
    if(n < 0) return 0;
    return n;
}

cv::Mat& NearestNeighbourInterpolationResizer::resize(cv::Mat& image){
    const double xRatio = static_cast<double>(image.rows) / static_cast<double>(xSize_), yRatio = static_cast<double>(image.cols) / static_cast<double>(ySize_);
    cv::Mat resizedImage(xSize_, ySize_, image.type());

        resizedImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int i = position[0], j = position[1];
        const double x = i*xRatio;
        const double y = j*yRatio;
        const int x0 = x, y0 = y; // floor 

        std::vector<std::pair<double, cv::Vec3b>> points;
        int x1, y1;
        if (x0 + 1 > image.rows)
            x1 = x0;
        else
            x1 = x0 + 1;
        
        if (y0 + 1 > image.cols)
            y1 = y0;
        else
            y1 = y0 + 1;

        points.push_back(std::make_pair(findDistance(x0, y0, position), image.at<cv::Vec3b>(x0, y0)));
        points.push_back(std::make_pair(findDistance(x0, y0, position), image.at<cv::Vec3b>(x0, y0)));
        points.push_back(std::make_pair(findDistance(x0, y0, position), image.at<cv::Vec3b>(x0, y0)));
        points.push_back(std::make_pair(findDistance(x0, y0, position), image.at<cv::Vec3b>(x0, y0)));

        std::sort(points.begin(), points.end(), [](const auto& x1, const auto& x2){
            return x1.first < x2.first;
        });
        pixel = points.front().second;
    });
    image = resizedImage;

   return image; 
}

const double NearestNeighbourInterpolationResizer::findDistance(const double x, const double y, const int position[]){
    return std::sqrt(std::pow(position[0] - x, 2) + std::pow(position[1] - y, 2));
}


cv::Mat EdgeAdaptiveInterpolationResizer::resize(cv::Mat& image){
    return image;
}

};      