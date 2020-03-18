#include<array>
#include<opencv2/opencv.hpp>

namespace POBR {
    
class ColorSpaceConverter {
  public:
    virtual cv::Mat& convert(cv::Mat& image) = 0; 
};

class BGR2HSVConverter : public ColorSpaceConverter {
  public:
    cv::Mat& convert(cv::Mat& image) override;

  private:
    class BGR2HSVPixelConverter {
      public:
        void operator()(cv::Vec3b& pixel, const int position[]) const;
      private: 
        uchar evalValue(const uchar red, const uchar green, const uchar blue) const;
        uchar evalSaturation(const uchar red, const uchar green, const uchar blue, const uchar value) const;
        uchar evalHue(const uchar red, const uchar green, const uchar blue, const uchar value) const;
    };
};

class ColorReducer {
  public:
    ColorReducer(const int divider) : divider_(divider) {};
    cv::Mat& reduce(cv::Mat& image);
  private:
    const std::array<uchar, 256> createLookupTable(const int divider);
    cv::Mat& reduceSingleChannel(cv::Mat& image, const std::array<uchar, 256> lookupTable);
    cv::Mat& reduceTripleChannel(cv::Mat& image, const std::array<uchar, 256> lookupTable);
    static const uchar GRAYSCALE = 1;
    static const uchar COLORSCALE = 3;
    const int divider_;
};

class HSV2BGRConverter : public ColorSpaceConverter {
  public:
    cv::Mat& convert(cv::Mat& image) override;
  
  private:
    class HSV2BGRPixelConverter {
      public:
        void operator()(cv::Vec3b& pixel, const int position[]) const;
    };
};

class HistogramEqualizer {
  public:
    cv::Mat& equalize(cv::Mat& image); 
  
  private:
    const std::array<int, 256> createHistogram(cv::Mat& image) const;
    const std::array<int, 256> createLookuptable(const std::array<int, 256> histogram, const int pixelCount) const;
    cv::Mat& applyLUT(cv::Mat& image, const std::array<int, 256> lut) const;
};

};