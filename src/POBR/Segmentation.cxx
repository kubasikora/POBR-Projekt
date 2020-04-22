#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include<stack>
#include<functional>
#include<iostream>

namespace POBR {

BoundingBox::BoundingBox(const int y, const int x, const int height, const int width) :
    y(y), x(x), height(height), width(width) {}

SegmentationUnit::SegmentationUnit(cv::Mat& red, cv::Mat& blue, cv::Mat& white, cv::Mat& yellow) : 
    colors_(mergeColorMatrices(red, blue, white, yellow)),
    states_(createInitialStateMatrix(red)) {}


cv::Mat_<Color> SegmentationUnit::mergeColorMatrices(const cv::Mat& red, const cv::Mat& blue, const cv::Mat& white, const cv::Mat& yellow){
    const int imageHeight = red.rows, imageWidth = red.cols;
    cv::Mat_<Color> colors(cv::Size(imageWidth, imageHeight)); 
    colors.forEach([](Color& pixel, const int[]) -> void {
        pixel = OTHER;
    });

    auto merger = [&colors](Color baseColor){
        return [&colors, baseColor](auto& pixel, const int position[]) -> void {
            if(pixel == 0)
                return;

            const int y = position[0], x = position[1];
            colors.at<Color>(y, x) = baseColor;  
        };
    };

    red.forEach<uchar>(merger(RED));
    blue.forEach<uchar>(merger(BLUE));
    white.forEach<uchar>(merger(WHITE));
    yellow.forEach<uchar>(merger(YELLOW));

    return colors;
}

cv::Mat_<State> SegmentationUnit::createInitialStateMatrix(const cv::Mat& exampleImage){
    const int imageHeight = exampleImage.rows, imageWidth = exampleImage.cols;
    cv::Mat_<State> states(cv::Size(imageWidth, imageHeight)); 
    states.forEach([](State& pixel, const int[]) -> void {
        pixel = NOTVISITED;
    });
    return states;
}

std::pair<std::vector<PointsList>, cv::Mat_<Color>> SegmentationUnit::segmentImage(){
    std::vector<PointsList>  segments;
    for(auto y = 1; y < colors_.rows - 1; ++y){
        for(auto x = 1; x < colors_.cols - 1; ++x){
            Color cell = colors_.at<Color>(y, x); State& cellState = states_.at<State>(y, x);
            if(cell == OTHER){
                cellState = MISSED;
                continue;
            }

            if(cellState == ADDED)
                continue;

            Color seed = cell; PointsList newSegment;
            std::stack<PointPosition> processList; processList.push(std::make_pair(y, x));
            while(!processList.empty()){
                PointPosition pixel = processList.top();
                State& pixelState = states_.at<State>(pixel.first, pixel.second); 
                Color& pixelColor = colors_.at<Color>(pixel.first, pixel.second);
                if(pixelColor == seed && pixelState != ADDED){
                    pixelState = ADDED; newSegment.push_back(pixel);
                    for(auto xOffset = -1; xOffset <= 1; ++xOffset){
                        for(auto yOffset = -1; yOffset <= 1; ++yOffset){
                            if(xOffset == 0 && yOffset == 0)
                                continue;
                            processList.push(std::make_pair(pixel.first + xOffset, pixel.second + yOffset));             
                        }
                    } 
                }
                processList.pop();
            }

            segments.push_back(newSegment);
        }
    }
    return std::make_pair(segments, colors_);
}


SegmentDescriptor::SegmentDescriptor(const PointsList points, const cv::Mat_<Color> image) : 
    points_(points), 
    area_(evaluateArea(points)), 
    color_(findSegmentColor(points, image)),
    boundingBox_(findBoundingBox(points)),
    cog_(findCenterOfGravity(points)),
    m11_(findGeometricMoment(points, 1, 1)) {}

void SegmentDescriptor::printDescriptorInfo(std::ostream& out){
    out << "Segment color: " << this->color_ << std::endl;
    out << "Segment area: " << this->area_ << std::endl;
    out << "Bounding box: (y: " << this->boundingBox_.y << ", x: " << this->boundingBox_.x << "), height = " << this->boundingBox_.height << ", width = " << this->boundingBox_.width << std::endl;
    out << "Center of gravity: (y: " << this->cog_.first << ", x: " << this->cog_.second << ")" << std::endl;  
    out << "m11: " << this->m11_ << std::endl;
    out << "===============================" << std::endl;
}

double SegmentDescriptor::evaluateArea(const PointsList points){
    return points.size();
}

Color SegmentDescriptor::findSegmentColor(const PointsList points, const cv::Mat_<Color> image){
    const PointPosition head = points.front();
    return image.at<Color>(head.first, head.second);
}

BoundingBox SegmentDescriptor::findBoundingBox(const PointsList points) {
    int minY, maxY, minX, maxX;
    PointPosition head = points.front();
    minY = head.first; maxY = head.first;
    minX = head.second; maxX = head.second;

    std::for_each(points.begin(), points.end(), [&](const PointPosition& point){
        const int y = point.first, x = point.second;
        if(y > maxY) maxY = y;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(x < minX) minX = x;
    });

    std::cout << minY << " : " << maxY << " / " << minX << " : " << maxX << std::endl; 

    return BoundingBox(minY, minX, maxY - minY, maxX - minX);
}

std::pair<double, double> SegmentDescriptor::findCenterOfGravity(const PointsList points){
    int xAccumulator = 0, yAccumulator = 0;
    std::for_each(points.begin(), points.end(), [&](const PointPosition& point){
        yAccumulator += point.first;
        xAccumulator += point.second;
    });

    return std::make_pair<double, double>(yAccumulator/points.size(), xAccumulator/points.size());
}

double SegmentDescriptor::findGeometricMoment(const PointsList points, const unsigned p, const unsigned q){
    double moment = 0.0;
    std::for_each(points.begin(), points.end(), [&moment, &p, &q](const PointPosition& point){
        const unsigned i = point.second, j = point.first;
        moment += std::pow(i, p) * std::pow(j, q);
    });

    return moment;
}

};
