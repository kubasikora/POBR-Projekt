#include<opencv2/opencv.hpp>

namespace POBR {

class ChannelInterval {
  public: 
    ChannelInterval(const uchar lower, const uchar upper);
    const uchar getLowerBoundary() const;
    const uchar getUpperBoundary() const;
    virtual ~ChannelInterval() = 0;

  protected:
    uchar lower_;
    uchar upper_;
};

class HueInterval : public ChannelInterval {
  public:
    HueInterval(const int lower, const int upper);
    ~HueInterval();
};

class SaturationInterval : public ChannelInterval {
  public:
    SaturationInterval(const uchar lower, const uchar upper);
    ~SaturationInterval();
};

class ValueInterval : public ChannelInterval {
  public:
    ValueInterval(const uchar lower, const uchar upper);
    ~ValueInterval();
};

class HSVMask {
  public:
    HSVMask(const HueInterval hue, const SaturationInterval saturation, const ValueInterval value);
    cv::Mat& apply(cv::Mat& image);

  private:
    cv::Vec3b& maskPixel(cv::Vec3b& pixel, const int position, const ChannelInterval& interval);

    const HueInterval hueInterval_;
    const SaturationInterval saturationInterval_;
    const ValueInterval valueInterval_;
};

};