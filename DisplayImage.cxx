#include<iostream>
#include<opencv2/opencv.hpp>

int main(int argc, char** argv){
    if(argc != 2){
        std::cout << "Usage: DisplayImage <imagePath>" << std::endl;
	return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], 1);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	return -1;
    }

    cv::namedWindow("Display image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display image", image);

    cv::waitKey(0);
    return 0;
}
