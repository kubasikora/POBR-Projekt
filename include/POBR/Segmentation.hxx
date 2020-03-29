#include<opencv2/opencv.hpp>

namespace POBR {

class HomogeneityTest {
    virtual bool testHomogeneity(const cv::Vec3b& p1, const cv::Vec3b& p2) = 0;
};

class ThresholdHomogeneityTest : public HomogeneityTest {
    ThresholdHomogeneityTest(const uchar threshold);
    bool testHomogeneity(const cv::Vec3b& p1, const cv::Vec3b& p2);
};

};