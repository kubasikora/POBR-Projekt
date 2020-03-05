#include<iostream>
#include<algorithm>
#include<opencv2/opencv.hpp>

void printImage(const cv::Mat& image){
    image.forEach<cv::Vec3b>([](const cv::Vec3b& pixel, const int position[]){
        std::cout << "BGR values of pixel (" << position[0] << "," << position[1] << ")" << std::endl;
        std::cout << "B: " << static_cast<int>(pixel[0]) << std::endl;
        std::cout << "G: " << static_cast<int>(pixel[1]) << std::endl;
        std::cout << "R: " << static_cast<int>(pixel[2]) << std::endl;
    });
}

class BGR2HSVConverter {
    cv::Vec3b& operator()(cv::Vec3b& pixel, const int position[]){
        
    }
};

cv::Mat& convertBGRtoHSV(cv::Mat& image){
    image.forEach<cv::Vec3b>([](cv::Vec3b& pixel, const int position[]){
        const uchar red = pixel[2], green = pixel[1], blue = pixel[0];
        const int minimum = std::min(std::min(red,  green), blue);
        
        int hue = 0; 
        uchar value = std::max(std::max(red, green), blue);
        uchar saturation = 0;

        if(minimum == value){
            hue = 0;
        } else {
            const uchar divider = value - minimum;
            if(red == value){
                hue = ((green - blue) * 60 / divider) + 0;
            }
            if(green == value){
                hue = ((blue - red) * 60 / divider) + 120;
            }
            if(blue == value){
                hue = ((red - green) * 60 / divider) + 240;
            }
        }

        if(hue < 0){
            hue += 360;
        }

        if(value){
            saturation = (value - minimum)*255 / value;
        }
        pixel = { static_cast<uchar>(hue / 2), saturation, value };
    });
    return image;
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


    // cv::namedWindow("Display image", cv::WINDOW_AUTOSIZE);
    // cv::imshow("Display image", image);

    std::cout << "converting start" << std::endl;

    std::vector<cv::Mat> channels(3);
    cv::Mat hsv = convertBGRtoHSV(image);
    cv::split(hsv, channels);
    cv::Mat h, s, v;
    h = channels[0];
    s = channels[1];
    v = channels[2];

    std::cout << "converting end" << std::endl;

    cv::namedWindow("H", cv::WINDOW_AUTOSIZE);
    cv::imshow("H", h);

    cv::Mat orig, origHSV;
    orig = cv::imread(argv[1], cv::IMREAD_COLOR);
    cv::cvtColor(orig, origHSV, cv::COLOR_BGR2HSV);
    std::vector<cv::Mat> channelsOriginal(3);

    cv::split(origHSV, channelsOriginal);
    cv::Mat ho, so, vo;
    ho = channelsOriginal[0];
    so = channelsOriginal[1];
    vo = channelsOriginal[2];

    cv::namedWindow("opencv H", cv::WINDOW_AUTOSIZE);
    cv::imshow("opencv H", ho);

    cv::namedWindow("S", cv::WINDOW_AUTOSIZE);
    cv::imshow("S", s);

    cv::namedWindow("SO", cv::WINDOW_AUTOSIZE);
    cv::imshow("SO", so);

    cv::waitKey(0);
    return 0;
}
