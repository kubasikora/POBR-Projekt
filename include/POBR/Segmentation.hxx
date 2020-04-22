#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

namespace POBR {

enum Color { RED, BLUE, WHITE, YELLOW, OTHER };
enum State { ADDED, CHECKED, NOTVISITED, MISSED };

typedef std::pair<int, int> PointPosition;
typedef std::vector<PointPosition> PointsList;
typedef std::vector<PointsList> SegmentList;

struct BoundingBox {
    BoundingBox();
    BoundingBox(const int y, const int x, const int height, const int width);
    int y, x, height, width;
};

class SegmentationUnit {
  public:
    SegmentationUnit(cv::Mat& red, cv::Mat& blue, cv::Mat& white, cv::Mat& yellow);
    std::pair<std::vector<PointsList>, cv::Mat_<Color>> segmentImage();

  private:
    static cv::Mat_<Color> mergeColorMatrices(const cv::Mat& red, const cv::Mat& blue, const cv::Mat& white, const cv::Mat& yellow);
    static cv::Mat_<State> createInitialStateMatrix(const cv::Mat& exampleImage);

    cv::Mat_<Color> colors_;
    cv::Mat_<State> states_;
};

class SegmentDescriptor {
  public:
    SegmentDescriptor(const PointsList points, const cv::Mat_<Color> image);
    void printDescriptorInfo(std::ostream& out);

    unsigned getArea();
    Color getColor();
    BoundingBox getBoundingBox();
    std::pair<double, double> getCenterOfGravity();
    double getm11();

  private:
    const PointsList points_;
    const cv::Mat_<Color> image_;
    
    unsigned area_;
    Color color_;
    BoundingBox boundingBox_;
    std::pair<double, double> cog_;
    double m11_;
    
    unsigned evaluateArea();
    Color findSegmentColor();
    BoundingBox findBoundingBox();
    std::pair<double, double> findCenterOfGravity();
    double findGeometricMoment(const unsigned p, const unsigned q);
};

};

