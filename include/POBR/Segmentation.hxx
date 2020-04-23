#ifndef __POBR_SEGMENTATION_HXX__
#define __POBR_SEGMENTATION_HXX__

#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

namespace POBR {

enum Color { RED, BLUE, WHITE, YELLOW, OTHER };
enum State { ADDED, CHECKED, NOTVISITED, MISSED };

std::string getColorName(Color c);

typedef std::pair<int, int> PointPosition;
typedef std::vector<PointPosition> PointsList;
typedef std::vector<PointsList> SegmentList;

struct BoundingBox {
    BoundingBox();
    BoundingBox(const int y, const int x, const int height, const int width);
    int y, x, height, width;
};

class ImageMerger {
  public:
    cv::Mat_<Color> mergeImage(std::vector<std::pair<Color, cv::Mat>> images);
};

class SegmentDescriptor {
  public:
    SegmentDescriptor(PointsList points, cv::Mat_<Color> image);
    void printDescriptorInfo(std::ostream& out);

    unsigned getArea();
    Color getColor();
    BoundingBox getBoundingBox();
    std::pair<double, double> getCenterOfGravity();
    double getwhRatio();

    static double getFiNorm(const SegmentDescriptor& s1, const SegmentDescriptor& s2);
    static double getFiNorm(const SegmentDescriptor& s1, const std::array<double, 5> s2);

  private:
    PointsList points_;
    cv::Mat_<Color> image_;
    
    double fi2_, fi3_, fi4_, fi5_, fi6_; 
    double m00_, m01_, m10_;
    double whRatio_;
    unsigned area_;
    Color color_;
    BoundingBox boundingBox_;
    std::pair<double, double> cog_;
    
    unsigned findArea();
    double findwhRatio();
    Color findSegmentColor();
    BoundingBox findBoundingBox();
    std::pair<double, double> findCenterOfGravity();
    double findRawGeometricMoment(const unsigned p, const unsigned q);
    double findCentralGeometricMoment(const unsigned p, const unsigned q);

    void findFiParameters();
};

class SegmentationUnit {
  public:
    SegmentationUnit(unsigned cutoffSize);
    std::vector<SegmentDescriptor> segmentImage(cv::Mat_<Color> colors);
    
  private:
    std::vector<PointsList> filterSegmentsBySize(std::vector<PointsList> pointLists);
    std::vector<PointsList> makeInitialSegmentation(cv::Mat_<Color> colors, cv::Mat_<State> states);
    std::vector<SegmentDescriptor> mapToDescriptors(std::vector<PointsList> pointLists, cv::Mat_<Color> colors);
    cv::Mat_<State> createInitialStateMatrix(const cv::Mat& exampleImage);
    const unsigned cutoffSize_;
};

};

#endif // __POBR_SEGMENTATION_HXX__
