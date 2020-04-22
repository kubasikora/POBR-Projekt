#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

namespace POBR {

enum Color { RED, BLUE, WHITE, YELLOW, OTHER };
enum State { ADDED, CHECKED, NOTVISITED, MISSED };

typedef std::pair<int, int> PointPosition;
typedef std::vector<PointPosition> PointsList;
typedef std::vector<PointsList> SegmentList;

class BoundingBox {
  public:
    BoundingBox(const int y, const int x, const int height, const int width);
    const int y, x, height, width;
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

    const PointsList points_;
    const double area_;
    const Color color_;
    const BoundingBox boundingBox_;
    const std::pair<double, double> cog_;
    const double m11_;

  private:
    static double evaluateArea(const PointsList points);
    static Color findSegmentColor(const PointsList points, const cv::Mat_<Color> image);
    static BoundingBox findBoundingBox(const PointsList points);
    static std::pair<double, double> findCenterOfGravity(const PointsList points);
    static double findGeometricMoment(const PointsList points, const unsigned p, const unsigned q);
};

};

