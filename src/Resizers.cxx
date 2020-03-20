#include"POBR/Preprocessing.hxx"

namespace POBR {

cv::Mat BilinearInterpolationResizer::resize(cv::Mat& image){
    const double xRatio = image.rows / xSize_, yRatio = image.cols / ySize_;
    cv::Mat resizedImage(xSize_, ySize_, image.type());

    resizedImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int i = position[0], j = position[1];
        const double x = i*xRatio, y = j*yRatio;
        const int x0 = x, y0 = y; // floor 
        const double dx = x - x0, dy = y - y0; // frac
        
        int x1, y1;
        if (x0 + 1 >= image.cols)
            x1 = x0;
        else
            x1 = x0 + 1;
        
        if (y0 + 1 >= image.rows)
            y1 = y0;
        else
            y1 = y0 + 1;

        auto interpolate = [](const double p1 , const double p2, const double d) -> const double {
            return p1*(1-d) + p2*d;
        };

        auto normalize = [](const double n) -> int {
            if(n > 255) return 255;
            if(n < 0) return 0;
            return n;
        };

        const cv::Vec3b p00 = image.at<cv::Vec3b>(x0, y0);
        const cv::Vec3b p01 = image.at<cv::Vec3b>(x0, y1);
        const cv::Vec3b p10 = image.at<cv::Vec3b>(x1, y0);
        const cv::Vec3b p11 = image.at<cv::Vec3b>(x1, y1);

        for(auto i = 0; i < pixel.channels; ++i){
            pixel[i] = normalize(interpolate(interpolate(p00[i], p10[i], dx), interpolate(p01[i], p11[i], dx), dy));
        }
    });
    // image = resizedImage;
    return resizedImage;
}

cv::Mat EdgeAdaptiveInterpolationResizer::resize(cv::Mat& image){
    return image;
}

};      