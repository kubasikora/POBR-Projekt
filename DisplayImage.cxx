#include<iostream>
#include<opencv2/opencv.hpp>

void printImage(const cv::Mat& image){
    image.forEach<cv::Vec3b>([](const cv::Vec3b& pixel, const int position[]){
        std::cout << "BGR values of pixel (" << position[0] << "," << position[1] << ")" << std::endl;
        std::cout << "B: " << static_cast<int>(pixel[0]) << std::endl;
        std::cout << "G: " << static_cast<int>(pixel[1]) << std::endl;
        std::cout << "R: " << static_cast<int>(pixel[2]) << std::endl;
    });
}

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

    std::cout << "BGR values of first pixel (0,0):" << std::endl;
    std::cout << "B: " << static_cast<int>(image.at<cv::Vec3b>(0,0)[0]) << std::endl;
    std::cout << "G: " << static_cast<int>(image.at<cv::Vec3b>(0,0)[1]) << std::endl;
    std::cout << "R: " << static_cast<int>(image.at<cv::Vec3b>(0,0)[2]) << std::endl;


    cv::namedWindow("Display image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display image", image);

    cv::waitKey(0);
    return 0;
}
