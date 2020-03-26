#include<algorithm>
#include<array>
#include<experimental/array>
#include<numeric>
#include"POBR/Preprocessing.hxx"

namespace POBR {

cv::Mat& BilinearInterpolationResizer::resize(cv::Mat& image){
    const double xRatio = static_cast<double>(image.cols) / static_cast<double>(xSize_), yRatio = static_cast<double>(image.rows) / static_cast<double>(ySize_);
    cv::Mat resizedImage(ySize_, xSize_, image.type());

    resizedImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int i = position[1], j = position[0];
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

        const cv::Vec3b p00 = image.at<cv::Vec3b>(y0, x0);
        const cv::Vec3b p01 = image.at<cv::Vec3b>(y1, x0);
        const cv::Vec3b p10 = image.at<cv::Vec3b>(y0, x1);
        const cv::Vec3b p11 = image.at<cv::Vec3b>(y1, x1);

        for(auto i = 0; i < pixel.channels; ++i){
            pixel[i] = norm(interpolate(interpolate(p00[i], p10[i], dx), interpolate(p01[i], p11[i], dx), dy));
        }
    });
    image = resizedImage;
    return image;
}

double BilinearInterpolationResizer::interpolate(const double p1, const double p2, const double d){
    return p1*(1-d) + p2*d;
}

uchar BilinearInterpolationResizer::norm(const double n){
    if(n > 255) return 255;
    if(n < 0) return 0;
    return n;
}

cv::Mat& NearestNeighbourInterpolationResizer::resize(cv::Mat& image){
    const double xRatio = static_cast<double>(image.cols) / static_cast<double>(xSize_), yRatio = static_cast<double>(image.rows) / static_cast<double>(ySize_);
    cv::Mat resizedImage(ySize_, xSize_, image.type());
    resizedImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int j = position[0], i = position[1];
        const double x = i*xRatio;
        const double y = j*yRatio;
        const int x0 = x, y0 = y; // floor 

        
        int x1, y1;
        if (x0 + 1 >= image.cols)
            x1 = x0;
        else
            x1 = x0 + 1;
        
        if (y0 + 1 >= image.rows)
            y1 = y0;
        else
            y1 = y0 + 1;

        
        const auto p00 = findDistance(y0, y0, position);
        const auto p10 = findDistance(y0, x1, position);
        const auto p01 = findDistance(y1, x0, position);
        const auto p11 = findDistance(y1, x1, position);

        const auto minim = std::min(std::min(p00, p10), std::min(p01, p11));

        if(p00 == minim)
            pixel = image.at<cv::Vec3b>(y0, x0);
        if(p10 == minim)
            pixel = image.at<cv::Vec3b>(y1, x1);
        if(p01 == minim)
            pixel = image.at<cv::Vec3b>(y1, x0);
        if(p11 == minim)
            pixel = image.at<cv::Vec3b>(y1, x1);
    });
    image = resizedImage;

   return image; 
}

double NearestNeighbourInterpolationResizer::findDistance(const double y, const double x, const int position[]){
    return std::sqrt(std::pow(position[1] - x, 2) + std::pow(position[0] - y, 2));
}

const std::array<double, 4> BicubicInterpolationResizer::evaluateCoefficients(const double x){
    std::array<double, 4> coeffs;
    coeffs[0] = ((A_*(x + 1) - 5*A_)*(x + 1) + 8*A_)*(x + 1) - 4*A_;
    coeffs[1] = ((A_ + 2)*x - (A_ + 3))*x*x + 1;
    coeffs[2] = ((A_ + 2)*(1 - x) - (A_ + 3))*(1 - x)*(1 - x) + 1;
    coeffs[3] = 1.0 - coeffs[0] - coeffs[1] - coeffs[2];

    return coeffs;
}

int BicubicInterpolationResizer::checkRowIfExist(const cv::Mat& image, const int x0, const int i){
    if(x0 + i > image.rows - 1 || x0 + i < 0)
        return x0;
    else 
        return x0 + i;
}

int BicubicInterpolationResizer::checkColIfExist(const cv::Mat& image, const int y0, const int i){
    if(y0 + i > image.cols - 1 || y0 + i < 0)
        return y0;
    else 
        return y0 + i;
}

std::array<std::array<double, 3>, 4> BicubicInterpolationResizer::createIntermediaryMatrix(){
    std::array<std::array<double, 3>, 4> intermediaryResults;
    for(auto j = 0; j < 4; ++j){
        for(auto k = 0; k < 3; ++k)
            intermediaryResults[j][k] = 0.0;
    }
    return intermediaryResults;
}

cv::Mat& BicubicInterpolationResizer::resize(cv::Mat& image){
    const double xRatio = static_cast<double>(image.cols) / static_cast<double>(xSize_), yRatio = static_cast<double>(image.rows) / static_cast<double>(ySize_);
    cv::Mat resizedImage(ySize_, xSize_, image.type());

    resizedImage.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int position[]){
        const int i = position[1], j = position[0];
        const double x = i*xRatio, y = j*yRatio;
        const int x0 = x, y0 = y; // floor 
        const double dx = x - x0, dy = y - y0; // frac
        
        const auto xCoeffs = evaluateCoefficients(dx);
        const auto yCoeffs = evaluateCoefficients(dy);

        // interpolate in x axis
        std::array<std::array<double, 3>, 4> intermediaryResults = createIntermediaryMatrix();
        for(auto i = 0; i < 4; ++i){
            for(auto j = 0; j < 4; ++j){
                const cv::Vec3b p = image.at<cv::Vec3b>(checkColIfExist(image, y0, i-1), checkRowIfExist(image, x0, j-1));
                for(auto k = 0; k < 3; ++k)
                    intermediaryResults[i][k] += p[k] * xCoeffs[j]; 
            }
        }   

        // interpolate in y axis
        std::array<double, 3> result = std::experimental::make_array<double>(0, 0, 0);
        for(auto i = 0; i < 4; ++i){
            for(auto j = 0; j < 3; ++j)
            result[j] += intermediaryResults[i][j] * yCoeffs[i];
        }

        // norm pixel
        for(auto i = 0; i < 3; ++i){
            pixel[i] = norm(result[i]);
        }
    });
    image = resizedImage;
    return image;
}

uchar BicubicInterpolationResizer::norm(const double n){
    if(n > 255) return 255;
    if(n < 0) return 0;
    return n;
}

};      