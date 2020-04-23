#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include"POBR/Masks.hxx"
#include"POBR/Filters.hxx"
#include"POBR/Preprocessing.hxx"
#include"POBR/Identification.hxx"
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

    POBR::BicubicInterpolationResizer bcr(1200, 900);
    image = bcr.resize(image);

    cv::Mat ogImage = image.clone();

    POBR::GaussianFilter gf(5, 3.0);
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

    POBR::HistogramEqualizer he;
    image = he.equalize(image);

    POBR::HSVMask redMask(POBR::HueInterval(-20, 20), POBR::SaturationInterval(150, 255), POBR::ValueInterval(100, 255));
    cv::Mat red = redMask.apply(image);
    POBR::DilationFilter d(3), dp(5), dd(7);
    POBR::ErosionFilter e(3), ep(5);
    red = d.filter(red);

    POBR::HSVMask blueMask(POBR::HueInterval(220, 300), POBR::SaturationInterval(70, 255), POBR::ValueInterval(0, 255));
    cv::Mat blue = blueMask.apply(image);

    POBR::HSVMask whiteMask(POBR::HueInterval(0, 360), POBR::SaturationInterval(0, 75), POBR::ValueInterval(100, 255));
    cv::Mat white = whiteMask.apply(image);

    POBR::HSVMask yellowMask(POBR::HueInterval(20, 60), POBR::SaturationInterval(80, 255), POBR::ValueInterval(150, 255));
    cv::Mat yellow = yellowMask.apply(image);
    yellow = dp.filter(yellow);
    yellow = e.filter(yellow);

    std::vector<std::pair<POBR::Color, cv::Mat>> images;
    images.push_back(std::make_pair(POBR::Color::RED, red));
    images.push_back(std::make_pair(POBR::Color::BLUE, blue));
    images.push_back(std::make_pair(POBR::Color::WHITE, white));
    images.push_back(std::make_pair(POBR::Color::YELLOW, yellow));

    POBR::ImageMerger im;
    auto maskedImage = im.mergeImage(images);

    POBR::SegmentationUnit su(150);
    auto descriptors = su.segmentImage(maskedImage);

    std::cout << "Extracted segments: " << descriptors.size() << std::endl;

    std::sort(descriptors.begin(), descriptors.end(), [](auto& v1, auto& v2){
        return v1.getArea() > v2.getArea();
    });

    std::map<POBR::Color, std::vector<POBR::SegmentDescriptor>> bins = {
        {POBR::Color::RED, {} },
        {POBR::Color::BLUE, {} },
        {POBR::Color::WHITE, {} },
        {POBR::Color::YELLOW, {} },
    };

    const std::array<double, 5> whiteModel = { 0.009737395, 2.18181e-7, 1.48058e-7, 4.23793e-15, 8.50314e-10 };
    const std::array<double, 5> blueModel = {0.5879375, 0.000248822, 5.08791e-5, 5.762582e-10, -1.1759715e-5};
    const std::array<double, 5> lowerBunModel = {0.666357, 3.059285e-5, 2.84446e-6, -3.87055e-12, -6.78627e-7};
    const std::array<double, 5> upperBunModel = {0.6692, 5.03532e-5, 4.84897e-6, -2.43564e-11, -2.41654e-6};
    const double whiteMomentDif = 0.15, blueMomentDif = 0.5, yellowMomentDif = 0.15;
    const double whiteWHDif = 0.9571055, whiteWHThreshold = 0.1867405*2;
    const double blueWHDif = 2.15142, blueWHThreshold = 0.748557*2;

    /* filter out segments based on hu's invariant moments */
    std::for_each(descriptors.begin(), descriptors.end(), [&](auto& segment){
        double dif = 0.0, difLo = 0.0, difUp = 0.0;
        switch(segment.getColor()){
          case POBR::Color::WHITE:
            dif = POBR::SegmentDescriptor::getFiNorm(segment, whiteModel);
            if(dif > whiteMomentDif || std::abs(whiteWHDif - segment.getwhRatio()) > whiteWHThreshold)
                break;

            bins[POBR::Color::WHITE].push_back(segment); break;

          case POBR::Color::RED:
            bins[POBR::Color::RED].push_back(segment); break;

          case POBR::Color::BLUE:
            dif = POBR::SegmentDescriptor::getFiNorm(segment, blueModel);
            if(dif > blueMomentDif || std::abs(blueWHDif - segment.getwhRatio()) > blueWHThreshold)
                break;

            bins[POBR::Color::BLUE].push_back(segment); break;

          case POBR::Color::YELLOW:
            difLo = POBR::SegmentDescriptor::getFiNorm(segment, lowerBunModel);
            difUp = POBR::SegmentDescriptor::getFiNorm(segment, upperBunModel);

            if(difLo > yellowMomentDif && difUp > yellowMomentDif)
                break;

            bins[POBR::Color::YELLOW].push_back(segment); break;

          default:
            break;
        };
    });

    std::for_each(bins[POBR::Color::WHITE].begin(), bins[POBR::Color::WHITE].end(), [&](auto& segment){
        segment.printDescriptorInfo(std::cout);
        POBR::BoundingBox bb = segment.getBoundingBox();
        if(bb.width == 0 || bb.height == 0)
            return;
        
        cv::Mat roi = ogImage(cv::Rect(bb.x, bb.y, bb.width, bb.height));
        cv::imshow("White segment", roi);
        
        cv::Mat rois = POBR::drawSegmentBoundary(ogImage, bb);
        cv::imshow("Picture", rois);
        cv::waitKey(-1);
    });

    std::for_each(bins[POBR::Color::RED].begin(), bins[POBR::Color::RED].end(), [&](auto& segment){
        segment.printDescriptorInfo(std::cout);
        POBR::BoundingBox bb = segment.getBoundingBox();
        if(bb.width == 0 || bb.height == 0)
            return;

        cv::Mat roi = ogImage(cv::Rect(bb.x, bb.y, bb.width, bb.height));
        cv::imshow("Red segment", roi);
        
        cv::Mat rois = POBR::drawSegmentBoundary(ogImage, bb);
        cv::imshow("Picture", rois);
        cv::waitKey(-1);
    });

    std::for_each(bins[POBR::Color::BLUE].begin(), bins[POBR::Color::BLUE].end(), [&](auto& segment){
        segment.printDescriptorInfo(std::cout);
        POBR::BoundingBox bb = segment.getBoundingBox();
        if(bb.width == 0 || bb.height == 0)
            return;
                
        cv::Mat roi = ogImage(cv::Rect(bb.x, bb.y, bb.width, bb.height));
        cv::imshow("Blue segment", roi);
        
        cv::Mat rois = POBR::drawSegmentBoundary(ogImage, bb);
        cv::imshow("Picture", rois);
        cv::waitKey(-1);
    });

    std::for_each(bins[POBR::Color::YELLOW].begin(), bins[POBR::Color::YELLOW].end(), [&](auto& segment){
        segment.printDescriptorInfo(std::cout);
        POBR::BoundingBox bb = segment.getBoundingBox();
        if(bb.width == 0 || bb.height == 0)
            return;
        
        cv::Mat roi = ogImage(cv::Rect(bb.x, bb.y, bb.width, bb.height));
        cv::imshow("Yellow segment", roi);
        
        cv::Mat rois = POBR::drawSegmentBoundary(ogImage, bb);
        cv::imshow("Picture", rois);
        cv::waitKey(-1);
    });
    
    return 0;
}
