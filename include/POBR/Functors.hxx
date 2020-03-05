#include<iostream>
#include<algorithm>
#include<opencv2/opencv.hpp>

namespace POBR {
    
class BGR2HSVPixelConverter {
  public:
    void operator()(cv::Vec3b& pixel, const int position[]) const;
  private: 
    uchar evalValue(const uchar red, const uchar green, const uchar blue) const;
    uchar evalSaturation(const uchar red, const uchar green, const uchar blue, const uchar value) const;
    uchar evalHue(const uchar red, const uchar green, const uchar blue, const uchar value) const;
};

};