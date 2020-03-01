#include<iostream>
#include<string>
#include<sstream>
#include<algorithm>
#include<opencv2/opencv.hpp>

cv::Mat& reduceColoursSingleChannel(cv::Mat& mat, const uchar* const lookupTable){
    std::for_each(mat.begin<uchar>(), mat.end<uchar>(), [&lookupTable](auto& cell){
        cell = lookupTable[cell];
    });
    return mat;
}

cv::Mat& reduceColoursTripleChannel(cv::Mat& mat, const uchar* const lookupTable){
    std::for_each(mat.begin<cv::Vec3b>(), mat.end<cv::Vec3b>(), [&lookupTable](auto& cell){
        cell[0] = lookupTable[cell[0]];
        cell[1] = lookupTable[cell[1]];
        cell[2] = lookupTable[cell[2]];
    });
    return mat;
}

cv::Mat& reduceColours(cv::Mat& mat, int divider){
    const int channels = mat.channels();
    const int grayScale = 1;
    const int colourScale = 3;

    uchar lookupTable[256];
    for(int i = 0; i < 256; ++i){
        lookupTable[i] = (uchar)(divider * (i/divider));
    }

    if(channels == grayScale){
        reduceColoursSingleChannel(mat, lookupTable);
    }
    else if(channels == colourScale){
        reduceColoursTripleChannel(mat, lookupTable);
    }
    return mat;
}

int main(int argc, char** argv){
    if(argc != 3){
        std::cout << "Usage: ColourSpaceReduction <imagePath> <divider>" << std::endl;
	    return -1;
    }

    std::string filename = argv[1];
    std::stringstream s;
    s << argv[2];
    int divider = 0;
    s >> divider;
    
    if(!s || !divider){
        std::cout << "Invalid divider" << std::endl;
    }

    cv::Mat image;
    image = cv::imread(filename, cv::IMREAD_COLOR);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	    return -1;
    }

    cv::namedWindow("Display image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display image", image);

    cv::Mat clone = image.clone();
    clone = reduceColours(clone, divider);
    
    cv::namedWindow("Reduced image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Reduced image", clone);

    cv::waitKey(0);
    return 0;
}
