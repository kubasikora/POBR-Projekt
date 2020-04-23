#ifndef __POBR_IDENTIFICATION_HXX__
#define __POBR_IDENTIFICATION_HXX__

#include"POBR/Segmentation.hxx"

namespace POBR {

cv::Mat drawSegmentBoundary(cv::Mat& image, BoundingBox bb);

};
#endif //__POBR_IDENTIFICATION_HXX__