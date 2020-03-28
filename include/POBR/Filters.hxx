#include<opencv2/opencv.hpp>
#include<algorithm>
#include<array>
#include<exception>

namespace POBR {

class Filter {
  public:
    virtual cv::Mat filter(cv::Mat& image) = 0;
};

class ConvolutionalFilter : public Filter {
  public:
    ConvolutionalFilter(const cv::Mat& kernel);
    virtual cv::Mat filter(cv::Mat& image) override;
  
  protected: 
    uchar normPixel(double i);
    
    const cv::Mat kernel_;
    const int filterOffsetX_;
    const int filterOffsetY_;
};

class MedianFilter : public Filter {
  public:
    MedianFilter(const int windowSize);
    virtual cv::Mat filter(cv::Mat& image) override;
  
  private:
    const int windowSize_;
    const int offset_;
};

class ErosionFilter : public Filter {
  public:
    ErosionFilter(const int windowSize);
    virtual cv::Mat filter(cv::Mat& image) override;
  
  private:
    const int windowSize_;
    const int offset_;
};

class DilationFilter : public Filter {
  public:
    DilationFilter(const int windowSize);
    virtual cv::Mat filter(cv::Mat& image) override;
  
  private:
    const int windowSize_;
    const int offset_;
};

class GaussianFilter : public ConvolutionalFilter {
  public:
    GaussianFilter(const int windowSizeX, const int windowSizeY, const double variance);

  private:
    static cv::Mat createKernel(const int windowSizeX, const int windowSizeY, const double variance);

    // const int filterOffsetX_;
    // const int filterOffsetY_;
    // const cv::Mat kernel_;

};

class InvalidWindowSizeException : public std::exception {};

}
