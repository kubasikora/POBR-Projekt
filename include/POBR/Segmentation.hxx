#include<opencv2/opencv.hpp>

namespace POBR {

class HomogeneityTest {
  public:
    virtual bool testHomogeneity(const cv::Vec3b& p1, const cv::Vec3b& p2) = 0;
};

class SimpleThresholdHomogeneityTest : public HomogeneityTest {
  public:
    SimpleThresholdHomogeneityTest(const uchar threshold);
    bool testHomogeneity(const cv::Vec3b& p1, const cv::Vec3b& p2);

  private:
    const uchar threshold_;
};

struct ImageSegment {

};

class RegionGrowingSegmentationUnit {
  public:
    RegionGrowingSegmentationUnit(SimpleThresholdHomogeneityTest test);
    void segmentImage(cv::Mat& image);

  private:
    const SimpleThresholdHomogeneityTest hTest_;
};

};