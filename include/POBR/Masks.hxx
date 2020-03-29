#include<opencv2/opencv.hpp>

namespace POBR {

class ChannelInterval {
  public: 
    ChannelInterval(const int lower, const int upper);
    int getLowerBoundary() const;
    int getUpperBoundary() const;
    virtual bool isInRange(const uchar pixel) const;
    virtual ~ChannelInterval() = 0;

  protected:
    int lower_;
    int upper_;
};

class HueInterval : public ChannelInterval {
  public:
    HueInterval(const int lower, const int upper);
    bool isInRange(const uchar pixel) const override;
    ~HueInterval();
};

class SaturationInterval : public ChannelInterval {
  public:
    SaturationInterval(const int lower, const int upper);
    ~SaturationInterval();
};

class ValueInterval : public ChannelInterval {
  public:
    ValueInterval(const int lower, const int upper);
    ~ValueInterval();
};

class HSVMask {
  public:
    HSVMask(const HueInterval hue, const SaturationInterval saturation, const ValueInterval value);
    cv::Mat apply(cv::Mat& image);

  private:
    cv::Vec3b& maskPixel(cv::Vec3b& pixel, const int position, const ChannelInterval& interval);

    const HueInterval hueInterval_;
    const SaturationInterval saturationInterval_;
    const ValueInterval valueInterval_;
};

class MaskApplier {
  public:
    MaskApplier& add(HSVMask mask);
    cv::Mat& apply(cv::Mat& image);
  private:
    std::vector<HSVMask> masks_;
};

class MaskFactory {
  public:
    MaskFactory(std::map<std::string, int> config) : config_(config) {};
    HSVMask build();

  private:
    const std::map<std::string, int> config_;
};

};