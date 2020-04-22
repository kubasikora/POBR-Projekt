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

    POBR::SegmentationUnit su(red, blue, white, yellow);
    auto segmentationResult = su.segmentImage();
    POBR::SegmentList pointLists = segmentationResult.first; 
    cv::Mat_<POBR::Color> colors = segmentationResult.second;

    std::sort(pointLists.begin(), pointLists.end(), [](auto& v1, auto&v2){ 
        return v1.size() > v2.size();
    });

    std::cout << "Biggest 10 segments: " << std::endl;
    for (auto x = 0; x < 10; ++x) {
        POBR::SegmentDescriptor descr(pointLists[x], colors);
        descr.printDescriptorInfo(std::cout);
    } 
    std::cout << std::endl;
    std::cout << "Image size: " << red.rows * red.cols << std::endl;
    std::cout << "Extracted segments: " << pointLists.size() << std::endl;

    for (auto x = 0; x < 10; ++x) {
        POBR::SegmentDescriptor seg(pointLists[x], colors);
        seg.printDescriptorInfo(std::cout);
        cv::Mat roi = image(cv::Rect(seg.boundingBox_.x, seg.boundingBox_.y, seg.boundingBox_.width, seg.boundingBox_.height));
        cv::imshow("seg", roi);
        cv::waitKey(-1);
    }
    
    return 0;
}
