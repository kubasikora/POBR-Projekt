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
    SegmentDescriptor(PointsList points, cv::Mat_<Color> image);
    void printDescriptorInfo(std::ostream& out);

    unsigned getArea();
    Color getColor();
    BoundingBox getBoundingBox();
    std::pair<double, double> getCenterOfGravity();
    double getwhRatio();

    double getfi1();
    double getfi2();
    double getfi3();
    double getfi4();
    double getfi5();
    double getfi6();
    double getfi7();

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

};

