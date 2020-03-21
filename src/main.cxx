#include<opencv2/opencv.hpp>
#include<yaml-cpp/yaml.h>
#include"Config.hxx"
#include"POBR/Preprocessing.hxx"
#include"POBR/Masks.hxx"

int main(int argc, char** argv){  
    if(argc < 2){
        std::cout << "Usage: MaskImage <imagePath>" << std::endl;
	    return -1;
    }

    std::unique_ptr<AppConfig> config;

    try {
        config = std::make_unique<AppConfig>("../config.yaml");
    } catch(YAML::BadFile& ex){
        std::cout << "Incorrect config file" << std::endl;
        return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	    return -1;
    }

    cv::Mat y;

    cv::resize(image, y, cv::Size(config->imageSize.first, config->imageSize.second), 0, 0, cv::INTER_CUBIC);

    POBR::BicubicInterpolationResizer resizer(config->imageSize, config->bicubic);
    image = resizer.resize(image);

    POBR::BGR2HSVConverter converter;
    converter.convert(image);

    if(config->equalizeHistogram) {
        POBR::HistogramEqualizer equalizer;
        equalizer.equalize(image);
    }
    if(config->reduceColors) {
        POBR::ColorReducer reducer(config->colorReducerRatio);
        reducer.reduce(image);
    }
    if(config->maskImage){
        POBR::MaskApplier masks;
        if(config->colorMasks["red"]){
            POBR::HSVMask redMask = POBR::MaskFactory(config->redMask).build();
            masks.add(redMask);
        }
        if(config->colorMasks["blue"]){
            POBR::HSVMask blueMask = POBR::MaskFactory(config->blueMask).build();
            masks.add(blueMask);
        }
        if(config->colorMasks["white"]){
            POBR::HSVMask whiteMask = POBR::MaskFactory(config->whiteMask).build();
            masks.add(whiteMask);
        }
        if(config->colorMasks["yellow"]){
            POBR::HSVMask yellowMask = POBR::MaskFactory(config->yellowMask).build();
            masks.add(yellowMask);
        }
        masks.apply(image);
    }
    
    POBR::HSV2BGRConverter reverter;
    reverter.convert(image);

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image);

    cv::waitKey(0);
    return 0;
}
