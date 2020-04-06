#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include<stack>
#include<functional>
#include<iostream>

namespace POBR {

SegmentationUnit::SegmentationUnit(cv::Mat& red, cv::Mat& blue, cv::Mat& white, cv::Mat& yellow) : 
    colors_(mergeColorMatrices(red, blue, white, yellow)),
    states_(createInitialStateMatrix(red)) {}


cv::Mat_<SegmentationUnit::Color> SegmentationUnit::mergeColorMatrices(const cv::Mat& red, const cv::Mat& blue, const cv::Mat& white, const cv::Mat& yellow){
    const int imageHeight = red.rows, imageWidth = red.cols;
    cv::Mat_<SegmentationUnit::Color> colors(cv::Size(imageWidth, imageHeight)); 
    std::cout << colors.rows << " : " << colors.cols << std::endl;
    colors.forEach([](SegmentationUnit::Color& pixel, const int[]) -> void {
        pixel = OTHER;
    });
    std::cout << colors.at<SegmentationUnit::Color>(10, 21) << std::endl;
    auto merger = [&colors](SegmentationUnit::Color baseColor){
        return [&colors, baseColor](auto& pixel, const int position[]) -> void {
            if(pixel == 0)
                return;

            std::cout << baseColor << std::endl;
            const int y = position[0], x = position[1];
            colors.at<SegmentationUnit::Color>(y, x) = baseColor;  
        };
    };

    red.forEach<uchar>(merger(RED));
    std::cout << colors.at<SegmentationUnit::Color>(10, 21) << std::endl;
    blue.forEach<uchar>(merger(BLUE));
    std::cout << colors.at<SegmentationUnit::Color>(10, 21) << std::endl;
    white.forEach<uchar>(merger(WHITE));
    std::cout << colors.at<SegmentationUnit::Color>(10, 21) << std::endl;
    yellow.forEach<uchar>(merger(YELLOW));
    std::cout << colors.at<SegmentationUnit::Color>(10, 21) << std::endl;

    return colors;
}

cv::Mat_<SegmentationUnit::State> SegmentationUnit::createInitialStateMatrix(const cv::Mat& exampleImage){
    const int imageHeight = exampleImage.rows, imageWidth = exampleImage.cols;
    cv::Mat_<SegmentationUnit::State> states(cv::Size(imageWidth, imageHeight)); 
    states.forEach([](SegmentationUnit::State& pixel, const int[]) -> void {
        pixel = NOTVISITED;
    });
    return states;
}

SegmentList SegmentationUnit::segmentImage(std::ostream& out){
    std::cout << colors_.at<SegmentationUnit::Color>(10, 21) << std::endl;
    std::cout << states_.at<SegmentationUnit::State>(10, 21) << std::endl;
    out << colors_ << std::endl;
    return SegmentList();
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
