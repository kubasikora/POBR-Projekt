#include<opencv2/opencv.hpp>
#include<algorithm>
#include<array>

namespace POBR {

class Filter {
  public:
    virtual cv::Mat filter(cv::Mat& image) = 0;
};

class ConvolutionalFilter : public Filter {
  public:
    ConvolutionalFilter(const cv::Mat& kernel);
    virtual cv::Mat filter(cv::Mat& image) override;
  private: 
    uchar normPixel(double i);
    
    const cv::Mat kernel_;
    const int filterOffsetX_;
    const int filterOffsetY_;
};

}
