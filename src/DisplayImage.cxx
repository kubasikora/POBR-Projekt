#include<iostream>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include"POBR/Preprocessing.hxx"
#include"POBR/Masks.hxx"

int main(int argc, char** argv){
    if(argc != 2){
        std::cout << "Usage: DisplayImage <imagePath>" << std::endl;
	    return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	    return -1;
    }

    cv::Mat cp;
    cv::cvtColor(image, cp, cv::COLOR_BGR2HSV);
    cv::cvtColor(cp, cp, cv::COLOR_HSV2BGR);

    cv::namedWindow("cvimage", cv::WINDOW_AUTOSIZE);
    cv::imshow("cvimage", cp);

    POBR::ColorReducer reducer;
    POBR::BGR2HSVConverter converter;
    POBR::HSV2BGRConverter reverter;
    POBR::HSVMask exampleMask(POBR::HueInterval(0, 360), POBR::SaturationInterval(0, 255), POBR::ValueInterval(200, 255));

    reducer.reduce(image, 10);
    converter.convert(image);
    exampleMask.apply(image);
    reverter.convert(image);

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image);

    cv::waitKey(0);
    return 0;

    std::vector<cv::Mat> hsv(3);
    cv::split(image, hsv);

    cv::namedWindow("H channel", cv::WINDOW_AUTOSIZE);
    cv::imshow("H channel", hsv[0]);

    cv::namedWindow("S channel", cv::WINDOW_AUTOSIZE);
    cv::imshow("S channel", hsv[1]);

    cv::namedWindow("V channel", cv::WINDOW_AUTOSIZE);
    cv::imshow("V channel", hsv[2]);

    cv::waitKey(0);
    return 0;
}
