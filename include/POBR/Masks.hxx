#include<opencv2/opencv.hpp>

namespace POBR {

class ChannelInterval {
  public:
    ChannelInterval(const uchar lower, const uchar upper);
    const uchar getLowerBoundary() const;
    const uchar getUpperBoundary() const;

  private:
    const uchar lower_;
    const uchar upper_;
};

class HSVMask {
  public:
    HSVMask(const ChannelInterval hue, const ChannelInterval saturation, const ChannelInterval value);
    cv::Mat& apply(cv::Mat& image);

  private:
    cv::Vec3b& maskPixel(cv::Vec3b& pixel, const int position, const ChannelInterval interval);

    const ChannelInterval hueInterval_;
    const ChannelInterval saturationInterval_;
    const ChannelInterval valueInterval_;
};

};