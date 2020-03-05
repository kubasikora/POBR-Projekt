#include<iostream>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include"POBR/Preprocessing.hxx"

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

    POBR::ColorReducer reductor;
    POBR::BGR2HSVConverter converter;

    reductor.reduce(image, 10);
    converter.convert(image);
     
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
