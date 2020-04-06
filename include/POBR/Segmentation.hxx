#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>

namespace POBR {

enum Color { RED, BLUE, WHITE, YELLOW, OTHER };
enum State { ADDED, CHECKED, NOTVISITED };

typedef std::pair<int, int> PointPosition;
typedef std::vector<PointPosition> Segment;
typedef std::vector<Segment> SegmentList;

class SegmentationUnit {
  public:
    SegmentationUnit(cv::Mat& red, cv::Mat& blue, cv::Mat& white, cv::Mat& yellow);
    SegmentList segmentImage();

  private:
    static cv::Mat_<Color> mergeColorMatrices(const cv::Mat& red, const cv::Mat& blue, const cv::Mat& white, const cv::Mat& yellow);
    static cv::Mat_<State> createInitialStateMatrix(const cv::Mat& exampleImage);

    cv::Mat_<Color> colors_;
    cv::Mat_<State> states_;
};

};