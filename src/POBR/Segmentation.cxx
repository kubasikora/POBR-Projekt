#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include<stack>
#include<functional>
#include<iostream>

namespace POBR {

std::string getColorName(Color c){
    switch (c){
      case Color::RED:
        return "RED";
      case Color::BLUE:
        return "BLUE";
      case Color::WHITE:
        return "WHITE";
      case Color::YELLOW:
        return "YELLOW";
      default:
        return "OTHER";    
    }
    return "OTHER";
}

BoundingBox::BoundingBox() :
    y(0), x(0), height(0), width(0) {} 

BoundingBox::BoundingBox(const int y, const int x, const int height, const int width) :
    y(y), x(x), height(height), width(width) {}

cv::Mat_<Color> ImageMerger::mergeImage(std::vector<std::pair<Color, cv::Mat>> images) {
    if(images.size() == 0)
        return cv::Mat_<Color>();
    
    std::pair<Color, cv::Mat> head = images.front();
    const unsigned imageHeight = head.second.rows, imageWidth = head.second.cols;

    cv::Mat_<Color> merged(cv::Size(imageWidth, imageHeight));
    merged.forEach([](Color& pixel, const int[]){
        pixel = OTHER;
    });

    auto mergerFunction = [&merged](Color baseColor){
        return [&merged, baseColor](auto& pixel, const int position[]){
            if(pixel == 0)
                return;
            
            const int y = position[0], x = position[1];
            merged.at<Color>(y, x) = baseColor;
        };
    };

    std::for_each(images.begin(), images.end(), [&](std::pair<Color, cv::Mat>& tpl){
        Color imageColor = tpl.first; cv::Mat filteredImage = tpl.second;
        filteredImage.forEach<uchar>(mergerFunction(imageColor));
    });

    return merged;
}

SegmentationUnit::SegmentationUnit(unsigned cutoffSize) : cutoffSize_(cutoffSize) {}

cv::Mat_<State> SegmentationUnit::createInitialStateMatrix(const cv::Mat& exampleImage){
    const int imageHeight = exampleImage.rows, imageWidth = exampleImage.cols;
    cv::Mat_<State> states(cv::Size(imageWidth, imageHeight)); 
    states.forEach([](State& pixel, const int[]) -> void {
        pixel = NOTVISITED;
    });
    return states;
}

std::vector<PointsList> SegmentationUnit::filterSegmentsBySize(std::vector<PointsList> pointLists){
    std::vector<PointsList> filteredLists;
    std::for_each(pointLists.begin(), pointLists.end(), [&](auto list){
        if(list.size() > cutoffSize_)
            filteredLists.push_back(list);
    });
    return filteredLists;
}

std::vector<SegmentDescriptor> SegmentationUnit::mapToDescriptors(std::vector<PointsList> pointLists, cv::Mat_<Color> colors){
    std::vector<SegmentDescriptor> descriptors;
    std::for_each(pointLists.begin(), pointLists.end(), [&](auto list){
            POBR::SegmentDescriptor segment(list, colors);
            descriptors.push_back(segment);
    });
    return descriptors;
}

std::vector<PointsList> SegmentationUnit::makeInitialSegmentation(cv::Mat_<Color> colors, cv::Mat_<State> states){
    std::vector<PointsList>  segments;
    for(auto y = 1; y < colors.rows - 1; ++y){
        for(auto x = 1; x < colors.cols - 1; ++x){
            Color cell = colors.at<Color>(y, x); State& cellState = states.at<State>(y, x);
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
                State& pixelState = states.at<State>(pixel.first, pixel.second); 
                Color& pixelColor = colors.at<Color>(pixel.first, pixel.second);
                if(pixelColor == seed && pixelState != ADDED){
                    pixelState = ADDED; newSegment.push_back(pixel);
                    for(auto xOffset = -1; xOffset <= 1; ++xOffset){
                        for(auto yOffset = -1; yOffset <= 1; ++yOffset){
                            if(xOffset == 0 && yOffset == 0)
                                continue;

                            const int pixelX = pixel.second + xOffset, pixelY = pixel.first + yOffset;
                            if(pixelX < 0 || pixelY < 0 || pixelX >= colors.cols || pixelY >= colors.rows)
                                continue;

                            processList.push(std::make_pair(pixel.first + yOffset, pixel.second + xOffset));             
                        }
                    } 
                }
                processList.pop();
            }
            segments.push_back(newSegment);
        }
    }
    return segments;
}

std::vector<SegmentDescriptor> SegmentationUnit::segmentImage(cv::Mat_<Color> colors){
    cv::Mat_<State> states = this->createInitialStateMatrix(colors);
    std::vector<PointsList> initialSegments = this->makeInitialSegmentation(colors, states);
    std::vector<PointsList> filteredSegments = this->filterSegmentsBySize(initialSegments);
    return this->mapToDescriptors(filteredSegments, colors);
}


SegmentDescriptor::SegmentDescriptor(PointsList points, cv::Mat_<Color> image) : 
    points_(points), image_(image) {
    this->boundingBox_ = findBoundingBox();
    this->area_ = findArea();
    this->color_ = findSegmentColor();
    this->whRatio_ = findwhRatio();
    this->cog_ = findCenterOfGravity();

    this->m10_ = findRawGeometricMoment(1, 0);
    this->m00_ = findRawGeometricMoment(0, 0);
    this->m01_ = findRawGeometricMoment(0, 1);

    this->findFiParameters();
}

unsigned SegmentDescriptor::getArea(){
    return this->area_;
}

Color SegmentDescriptor::getColor(){
    return this->color_;
}

BoundingBox SegmentDescriptor::getBoundingBox(){
    return this->boundingBox_;
}

double SegmentDescriptor::getwhRatio(){
    return this->whRatio_;
}

std::pair<double, double> SegmentDescriptor::getCenterOfGravity(){
    return this->cog_;
}

void SegmentDescriptor::printDescriptorInfo(std::ostream& out){
    out << "Segment color: " << getColorName(this->color_) << std::endl;
    out << "Segment area: " << this->area_ << std::endl;
    out << "Bounding box: (y: " << this->boundingBox_.y << ", x: " << this->boundingBox_.x << "), height = " << this->boundingBox_.height << ", width = " << this->boundingBox_.width << std::endl;
    out << "Center of gravity: (y: " << this->cog_.first << ", x: " << this->cog_.second << ")" << std::endl;  
    out << "Width to height ratio: " << this->whRatio_ << std::endl;

    out << "fi2: " << this->fi2_ << std::endl;
    out << "fi3: " << this->fi3_ << std::endl;
    out << "fi4: " << this->fi4_ << std::endl;
    out << "fi5: " << this->fi5_ << std::endl;
    out << "fi6: " << this->fi6_ << std::endl;

    out << "===============================" << std::endl;
}

unsigned SegmentDescriptor::findArea(){
    return this->points_.size();
}

double SegmentDescriptor::findwhRatio(){
    return static_cast<double>(this->boundingBox_.width) / (this->boundingBox_.height != 0 ? static_cast<double>(this->boundingBox_.height) : 1.0);
}

Color SegmentDescriptor::findSegmentColor(){
    const PointPosition head = points_.front();
    return image_.at<Color>(head.first, head.second);
}

BoundingBox SegmentDescriptor::findBoundingBox() {
    int minY, maxY, minX, maxX;
    PointPosition head = points_.front();
    minY = head.first; maxY = head.first;
    minX = head.second; maxX = head.second;

    std::for_each(points_.begin(), points_.end(), [&](const PointPosition& point){
        const int y = point.first, x = point.second;
        if(y > maxY) maxY = y;
        if(y < minY) minY = y;
        if(x > maxX) maxX = x;
        if(x < minX) minX = x;
    });

    return BoundingBox(minY, minX, maxY - minY, maxX - minX);
}

std::pair<double, double> SegmentDescriptor::findCenterOfGravity(){
    int xAccumulator = 0, yAccumulator = 0;
    std::for_each(points_.begin(), points_.end(), [&](const PointPosition& point){
        yAccumulator += point.first;
        xAccumulator += point.second;
    });

    return std::make_pair<double, double>(yAccumulator/points_.size(), xAccumulator/points_.size());
}

double SegmentDescriptor::findRawGeometricMoment(const unsigned p, const unsigned q){
    double moment = 0.0;
    std::for_each(points_.begin(), points_.end(), [&moment, &p, &q](const PointPosition& point){
        const unsigned i = point.second, j = point.first;
        moment += std::pow(i, p) * std::pow(j, q);
    });

    return moment;
}

double SegmentDescriptor::findCentralGeometricMoment(const unsigned p, const unsigned q){
    double moment = 0.0;
    const double ym = this->m01_ / this->m00_, xm = this->m10_ / this->m00_; 
    std::for_each(points_.begin(), points_.end(), [&](const PointPosition& point){
        const unsigned i = point.second, j = point.first;
        moment += std::pow(i - xm, p) * std::pow(j - ym, q);
    });

    return moment;
}

void SegmentDescriptor::findFiParameters(){
    double u20 = findCentralGeometricMoment(2,0);
    double u02 = findCentralGeometricMoment(0,2);
    double u11 = findCentralGeometricMoment(1,1);
    double u30 = findCentralGeometricMoment(3,0);
    double u12 = findCentralGeometricMoment(1,2);
    double u03 = findCentralGeometricMoment(0,3); 
    double u21 = findCentralGeometricMoment(2,1);
    
    fi2_ = std::pow(u20 - u02, 2) + 4*std::pow(u11, 2);
    fi3_ = std::pow(u30 - 3*u12, 2) + std::pow(3*u21 - u03, 2);
    fi4_ = std::pow(u30 + u12, 2) + std::pow(u21 + u03, 2);
    fi5_ = (u30 - 3*u12)*(u30 + u12)*(std::pow(u30 + u12, 2) - 3*std::pow(u21 + u03, 2)) + (3*u21 - u03)*(u21 + u03)*(3*std::pow(u30 + u12, 2) - std::pow(u21 + u03, 2));
    fi6_ = (u20 - u02)*(std::pow(u30 + u12, 2) - std::pow(u21 + u03, 2)) + 4*u11*(u30 + u12)*(u21 + u03);

    double r = std::sqrt(u20 + u02);
    fi2_ = fi2_ / std::pow(r, 4);
    fi3_ = fi3_ / std::pow(r, 6);
    fi4_ = fi4_ / std::pow(r, 6);
    fi5_ = fi5_ / std::pow(r, 12);
    fi6_ = fi6_ / std::pow(r, 8);
}

double SegmentDescriptor::getFiNorm(const SegmentDescriptor& s1, const SegmentDescriptor& s2){
    double fi2Diff = std::pow(s1.fi2_ - s2.fi2_, 2);
    double fi3Diff = std::pow(s1.fi3_ - s2.fi3_, 2);
    double fi4Diff = std::pow(s1.fi4_ - s2.fi4_, 2);
    double fi5Diff = std::pow(s1.fi5_ - s2.fi5_, 2);
    double fi6Diff = std::pow(s1.fi6_ - s2.fi6_, 2);

    return std::sqrt(fi2Diff + fi3Diff + fi4Diff + fi5Diff + fi6Diff);
}

double SegmentDescriptor::getFiNorm(const SegmentDescriptor& s1, const std::array<double, 5> s2){
    double fi2Diff = std::pow(s1.fi2_ - s2[0], 2);
    double fi3Diff = std::pow(s1.fi3_ - s2[1], 2);
    double fi4Diff = std::pow(s1.fi4_ - s2[2], 2);
    double fi5Diff = std::pow(s1.fi5_ - s2[3], 2);
    double fi6Diff = std::pow(s1.fi6_ - s2[4], 2);

    return std::sqrt(fi2Diff + fi3Diff + fi4Diff + fi5Diff + fi6Diff);
}

SegmentDescriptor SegmentDescriptor::operator+(SegmentDescriptor& b){
    PointsList accumulatedPoints;
    std::copy(this->points_.begin(), this->points_.end(), std::back_inserter(accumulatedPoints));   
    std::copy(b.points_.begin(), b.points_.end(), std::back_inserter(accumulatedPoints)); 

    std::sort(accumulatedPoints.begin(), accumulatedPoints.end());
    accumulatedPoints.erase(std::unique(accumulatedPoints.begin(), accumulatedPoints.end()), accumulatedPoints.end() );
    return SegmentDescriptor(accumulatedPoints, this->image_);
}

};
