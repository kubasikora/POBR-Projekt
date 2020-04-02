#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include<stack>

namespace POBR {

SimpleThresholdHomogeneityTest::SimpleThresholdHomogeneityTest(const uchar threshold) :
    threshold_(threshold) {}

bool SimpleThresholdHomogeneityTest::testHomogeneity(const cv::Vec3b& p1, const cv::Vec3b& p2){
    const double r1 = p1[2], r2 = p2[2], g1 = p1[1], g2 = p2[1], b1 = p1[0], b2 = p2[0];
    const double diff = std::sqrt(std::pow(r1 - r2, 2) + std::pow(g1 -g2, 2) + std::pow(b1 - b2, 2));\
    return diff <= threshold_;
}

RegionGrowingSegmentationUnit::RegionGrowingSegmentationUnit(SimpleThresholdHomogeneityTest test) :
    hTest_(test) {}

void RegionGrowingSegmentationUnit::segmentImage(cv::Mat& image){
    std::stack<cv::Vec3b> points;

    /* 
        ******** spliting *********
        set ProcessList = image
        repeat 
            extract first element of ProcessList
            if region is uniform 
                add to RegionList
            else
                split region into 4 sub-regions and add to ProcessList
        until all regions removed from ProcessList


        ********* merging *********
        repeat 
            extract each region from RegionList
            traverse remainder of list to find similar region 
            if they are neighbours 
                merge regions
        until no merges are possible
    */
}

};
