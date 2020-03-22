#include<array>
#include<opencv2/opencv.hpp>

namespace POBR {
    
class ColorSpaceConverter {
  public:
    virtual cv::Mat& convert(cv::Mat& image) = 0; 
    virtual ~ColorSpaceConverter() {}
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

class ImageResizer {
  virtual cv::Mat& resize(cv::Mat& image) = 0;
};

class NearestNeighbourInterpolationResizer : ImageResizer {
  public:
    NearestNeighbourInterpolationResizer(const int x, const int y) :
      xSize_(x), ySize_(y) {}
    NearestNeighbourInterpolationResizer(const std::pair<int, int> size) :
      xSize_(size.first), ySize_(size.second) {}
    virtual cv::Mat& resize(cv::Mat& image);

  private:  
    double findDistance(const double x, const double y, const int* position);

    const int xSize_;
    const int ySize_;
};

class BilinearInterpolationResizer : ImageResizer {
  public:
    BilinearInterpolationResizer(const int x, const int y) : 
      xSize_(x), ySize_(y) {}
    BilinearInterpolationResizer(const std::pair<int, int> size) :
      xSize_(size.first), ySize_(size.second) {}
    virtual cv::Mat& resize(cv::Mat& image);

  private:
    double interpolate(const double p1, const double p2, const double d);
    uchar norm(const double n);

    const int xSize_;
    const int ySize_;
};

class BicubicInterpolationResizer : ImageResizer {
  public:
    BicubicInterpolationResizer(const int x, const int y) : 
      xSize_(x), ySize_(y) {}
    BicubicInterpolationResizer(const std::pair<int, int> size) :
      xSize_(size.first), ySize_(size.second) {}
    virtual cv::Mat& resize(cv::Mat& image);

  private:
    const std::array<double, 4> evaluateCoefficients(const double x);
    std::array<std::array<double, 3>, 4> createIntermediaryMatrix();
    uchar norm(const double n);
    int checkRowIfExist(const cv::Mat& image, const int x0, const int i);
    int checkColIfExist(const cv::Mat& image, const int yo, const int i);

    const int xSize_;
    const int ySize_;
    const double A_ = -0.75;
};

};