#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include"POBR/Masks.hxx"
#include"POBR/Filters.hxx"
#include"POBR/Preprocessing.hxx"
#include<stack>
#include<iostream>
#include<fstream>

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Usage: Segmentation <imagePath> " << std::endl;
	    return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	    return -1;
    };

    POBR::BicubicInterpolationResizer bcr(1000, 1000);
    image = bcr.resize(image);

    cv::Mat ogImage = image.clone();

    POBR::GaussianFilter gf(5, 1.0);
    image = gf.filter(image);
    cv::Mat highPassKernel = (cv::Mat_<double>(5,5) << 0, 0, 0, 0, 0, 
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, -0.5, 5, -0.5, 0,
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, 0, 0, 0, 0);
    POBR::ConvolutionalFilter highPass(highPassKernel);
    image = highPass.filter(image);
    
    POBR::BGR2HSVConverter converter;
    converter.convert(image);

    POBR::HSVMask redMask(POBR::HueInterval(-20, 20), POBR::SaturationInterval(180, 255), POBR::ValueInterval(100, 255));
    cv::Mat red = redMask.apply(image);

    POBR::HSVMask blueMask(POBR::HueInterval(220, 260), POBR::SaturationInterval(100, 255), POBR::ValueInterval(0, 255));
    cv::Mat blue = blueMask.apply(image);

    POBR::HSVMask whiteMask(POBR::HueInterval(0, 360), POBR::SaturationInterval(0, 40), POBR::ValueInterval(100, 255));
    cv::Mat white = whiteMask.apply(image);

    POBR::HSVMask yellowMask(POBR::HueInterval(20, 60), POBR::SaturationInterval(70, 255), POBR::ValueInterval(150, 255));
    cv::Mat yellow = yellowMask.apply(image);

    std::vector<std::pair<POBR::Color, cv::Mat>> images;
    images.push_back(std::make_pair(POBR::Color::RED, red));
    images.push_back(std::make_pair(POBR::Color::BLUE, blue));
    images.push_back(std::make_pair(POBR::Color::WHITE, white));
    images.push_back(std::make_pair(POBR::Color::YELLOW, yellow));

    POBR::ImageMerger im;
    auto maskedImage = im.mergeImage(images);

    POBR::SegmentationUnit su(100);
    auto descriptors = su.segmentImage(maskedImage);

    std::cout << "Extracted segments: " << descriptors.size() << std::endl;

    std::sort(descriptors.begin(), descriptors.end(), [](auto& v1, auto& v2){
        return v1.getArea() > v2.getArea();
    });

    std::for_each(descriptors.begin(), descriptors.end(), [&](auto& segment){
        segment.printDescriptorInfo(std::cout);
        POBR::BoundingBox bb = segment.getBoundingBox();
        if(bb.width == 0 || bb.height == 0)
            return;
        cv::Mat roi = ogImage(cv::Rect(bb.x, bb.y, bb.width, bb.height));
        cv::imshow("Segment", roi);
        cv::waitKey(-1);
    });

    return 0;
}
