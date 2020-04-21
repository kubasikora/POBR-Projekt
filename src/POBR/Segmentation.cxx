#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include<stack>
#include<functional>
#include<iostream>

namespace POBR {

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

SegmentList SegmentationUnit::segmentImage(){
    SegmentList segments;
    for(auto y = 1; y < colors_.rows - 1; ++y){
        for(auto x = 1; x < colors_.cols - 1; ++x){
            Color cell = colors_.at<Color>(y, x);
            State& cellState = states_.at<State>(y, x);
            if(cell == OTHER){
                cellState = MISSED;
                continue;
            }

            if(cellState == ADDED)
                continue;

            Color seed = cell; Segment newSegment;
            std::cout << "Seed: " << y << " : " << x << " = " << seed << std::endl;

            std::stack<PointPosition> processList; processList.push(std::make_pair(y, x));
            while(!processList.empty()){
                PointPosition pixel = processList.top();
                State& pixelState = states_.at<State>(pixel.first, pixel.second);
                Color& pixelColor = colors_.at<Color>(pixel.first, pixel.second);
                if(pixelColor == seed && pixelState != ADDED){
                    pixelState = ADDED;
                    newSegment.push_back(pixel);
                
                    processList.push(std::make_pair(pixel.first, pixel.second - 1));
                    processList.push(std::make_pair(pixel.first, pixel.second + 1));
                    processList.push(std::make_pair(pixel.first - 1, pixel.second));
                    processList.push(std::make_pair(pixel.first + 1, pixel.second));
                    processList.push(std::make_pair(pixel.first - 1, pixel.second - 1));
                    processList.push(std::make_pair(pixel.first - 1, pixel.second + 1));
                    processList.push(std::make_pair(pixel.first + 1, pixel.second + 1));
                    processList.push(std::make_pair(pixel.first + 1, pixel.second - 1));
                }
                processList.pop();
            }

            segments.push_back(newSegment);
        }
    }

    return segments;
    /*
        1. stworzyć macierz obrazka typu Mat_<Color> 
        2. stworzyć macierz stanów typu Mat_<State>
        3. dla każdego piksela jeśli nie jest Color::OTHER 
            3.1 zapisz ziarno jako piksel
            3.2 stworz stos jako jedyny element wejsciowy piksel
            3.3 dopoki nie skonczą się piksele na stosie
                3.3.1 pobierz piksel
                3.3.2 jesli jest taki sam jak ziarno to zaznacz jako dodany, dodaj do boxa i pobierz sasiadow na stos jesli nie byli sprawdzeni
                3.3.3 jesli nie jest taki sam jak ziarno to zaznacz jako sprawdzony
            3.4 zwroc box
        4. zwroc wszystkie boxy
    */
}

};
