#include<array>
#include<opencv2/opencv.hpp>

namespace POBR {
    
class BGR2HSVConverter {
  public:
    cv::Mat& convert(cv::Mat& image);

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
    cv::Mat& reduce(cv::Mat& image, const int divider);
  private:
    const std::array<uchar, 256> createLookupTable(const int divider);
    cv::Mat& reduceSingleChannel(cv::Mat& image, const std::array<uchar, 256> lookupTable);
    cv::Mat& reduceTripleChannel(cv::Mat& image, const std::array<uchar, 256> lookupTable);
    static const uchar GRAYSCALE = 1;
    static const uchar COLORSCALE = 3;
};

};