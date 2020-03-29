#include<opencv2/opencv.hpp>
#include<yaml-cpp/yaml.h>
#include"Config.hxx"
#include"POBR/Preprocessing.hxx"
#include"POBR/Masks.hxx"
#include"POBR/Filters.hxx"

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

    // preprocessing -> prescaling
    switch (config->preprocessing.algorithm){
      case AlgorithmType::NearestNeighbour:{
          POBR::NearestNeighbourInterpolationResizer r1(config->preprocessing.size);
          r1.resize(image);
          break;
        }  

      case AlgorithmType::Bilinear: {
          POBR::BilinearInterpolationResizer r2(config->preprocessing.size);
          r2.resize(image);    
          break;
        }

      case AlgorithmType::Bicubic: default: {
          POBR::BicubicInterpolationResizer r3(config->preprocessing.size);
          r3.resize(image);
          break;
        }
    }

    // preprocessing -> reduction of colors
    if(config->preprocessing.reduceColors) {
        POBR::ColorReducer reducer(config->preprocessing.colorReducerRatio);
        reducer.reduce(image);
    }
    
    POBR::GaussianFilter gf(3, 1.0);
    image = gf.filter(image);

    cv::Mat highPassKernel = (cv::Mat_<double>(5,5) << 0, 0, 0, 0, 0, 
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, -0.5, 5, -0.5, 0,
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, 0, 0, 0, 0);
    POBR::ConvolutionalFilter highPass(highPassKernel);
    image = highPass.filter(image);

    // conversion -> to hsv
    POBR::BGR2HSVConverter converter;
    converter.convert(image);

    // preprocessing -> histogram equalization
    if(config->preprocessing.equalizeHistogram) {
        POBR::HistogramEqualizer equalizer;
        equalizer.equalize(image);
    }


    // segmentation -> thresholding
    cv::Mat result(image.size(), image.type());
    if(config->segmentation.maskImage){
        POBR::MaskApplier masks;
        if(config->segmentation.colorMasks["red"]){
            cv::Mat redImage = image.clone();
            POBR::HSVMask redMask = POBR::MaskFactory(config->segmentation.redMask).build();
            redImage = redMask.apply(redImage);

            POBR::ErosionFilter ef(3);
            POBR::DilationFilter df(3);
            redImage = ef.filter(redImage);
            redImage = df.filter(redImage);
            redImage = df.filter(redImage);

            // extract red segments
        
        }
        if(config->segmentation.colorMasks["blue"]){
            cv::Mat blueImage = image.clone();
            POBR::HSVMask blueMask = POBR::MaskFactory(config->segmentation.blueMask).build();
            blueImage = blueMask.apply(blueImage);

            POBR::ErosionFilter ef(3);
            POBR::DilationFilter df(7);
            blueImage = ef.filter(blueImage);
            blueImage = df.filter(blueImage);
            blueImage = ef.filter(blueImage);

            // extract blue segments
        }
        if(config->segmentation.colorMasks["white"]){
            cv::Mat whiteImage = image.clone();
            POBR::HSVMask whiteMask = POBR::MaskFactory(config->segmentation.whiteMask).build();
            whiteImage = whiteMask.apply(whiteImage);

            POBR::ErosionFilter ef(3);
            POBR::DilationFilter df(3);
            
            whiteImage = ef.filter(whiteImage);
            whiteImage = df.filter(whiteImage);
            whiteImage = df.filter(whiteImage);

            // extract white segments
        }
        if(config->segmentation.colorMasks["yellow"]){
            cv::Mat yellowImage = image.clone();
            POBR::HSVMask yellowMask = POBR::MaskFactory(config->segmentation.yellowMask).build();
            yellowImage = yellowMask.apply(yellowImage);

            POBR::ErosionFilter ef(3);
            POBR::DilationFilter df(3);
        
            yellowImage = ef.filter(yellowImage);
            yellowImage = df.filter(yellowImage);
            yellowImage = df.filter(yellowImage);

            // extract yellow segments
        }
    }
    
    image = result;
    // conversion -> to rgb
    POBR::HSV2BGRConverter reverter;
    reverter.convert(image);

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::imshow("image", image);

    cv::waitKey(0);
    return 0;
}
