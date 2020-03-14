#include<iostream>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include"POBR/Preprocessing.hxx"
#include"POBR/Masks.hxx"

int main(int argc, char** argv){    
    if(argc < 2){
        std::cout << "Usage: MaskImage <imagePath>" << std::endl;
	    return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	    return -1;
    }

    cv::resize(image, image, cv::Size(800,600));

    POBR::ColorReducer reducer;
    POBR::BGR2HSVConverter converter;
    POBR::HSV2BGRConverter reverter;
    POBR::HSVMask exampleMask(POBR::HueInterval(-20, 20), POBR::SaturationInterval(0, 255), POBR::ValueInterval(150, 255));
    
    reducer.reduce(image, 10);
    converter.convert(image);
    exampleMask.apply(image);
    reverter.convert(image);

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image);

    cv::waitKey(0);
    return 0;
}
