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

cv::Mat& convertRGBtoHSV(cv::Mat& image){
    image.forEach<cv::Vec3b>([](cv::Vec3b& pixel, const int channels[]){
        const uchar red = pixel[2];
        const uchar green = pixel[1];
        const uchar blue = pixel[0];
        const uchar temp = std::min(std::min(red,  green), blue);

        uchar value = std::max(std::max(red, green), blue);
        uchar hue = 0; 
        uchar saturation = 0;
        if(temp == value){
            hue = 0;
        } else {
            if(red == value){
                hue = ((green - blue) * 60 / (value - temp)) / 2 + 0;
            }
            if(green == value){
                hue = ((blue - red) * 60 / (value - temp)) / 2 + 60;
            }
            if(blue == value){
                hue = ((red - green) * 60 / (value - temp)) / 2 + 120;
            }
        }

        if(hue < 0){
            hue += 180;
        }

        if(value){
            saturation = (value - temp)*100 / value;
        }

        value = (100*value) / 255;

        pixel[0] = hue;
        pixel[1] = saturation;
        pixel[2] = value;
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


    cv::namedWindow("Display image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display image", image);

    std::cout << "converting start" << std::endl;

    std::vector<cv::Mat> channels(3);
    cv::split(convertRGBtoHSV(image), channels);
    cv::Mat h, s, v;
    h = channels[0];
    s = channels[1];
    v = channels[2];

    std::cout << "converting end" << std::endl;

    cv::namedWindow("H", cv::WINDOW_AUTOSIZE);
    cv::imshow("H", h);

    cv::namedWindow("S", cv::WINDOW_AUTOSIZE);
    cv::imshow("S", s);

    cv::namedWindow("V", cv::WINDOW_AUTOSIZE);
    cv::imshow("V", v);

    cv::waitKey(0);
    return 0;
}
