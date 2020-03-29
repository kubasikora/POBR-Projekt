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

    POBR::ErosionFilter ef(3);
    POBR::DilationFilter df(3);


    // segmentation -> thresholding
    cv::Mat result(image.size(), image.type());
    if(config->segmentation.maskImage){
        POBR::MaskApplier masks;
        if(config->segmentation.colorMasks["red"]){
            cv::Mat partialImage = image.clone();
            POBR::HSVMask redMask = POBR::MaskFactory(config->segmentation.redMask).build();
            redMask.apply(partialImage);
            partialImage = df.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = df.filter(partialImage);
            // cv::imshow("red", partialImage);
            // cv::waitKey(-1);
            result = result | partialImage;
            // masks.add(redMask);
        }
        if(config->segmentation.colorMasks["blue"]){
            cv::Mat partialImage = image.clone();
            POBR::HSVMask blueMask = POBR::MaskFactory(config->segmentation.blueMask).build();
            blueMask.apply(partialImage);
            partialImage = df.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = df.filter(partialImage);
            // cv::imshow("blue", partialImage);
            // cv::waitKey(-1);
            result = result | partialImage;
            // masks.add(blueMask);
        }
        if(config->segmentation.colorMasks["white"]){
            cv::Mat partialImage = image.clone();
            POBR::HSVMask whiteMask = POBR::MaskFactory(config->segmentation.whiteMask).build();
            whiteMask.apply(partialImage);
            partialImage = df.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = df.filter(partialImage);
            // cv::imshow("white", partialImage);
            // cv::waitKey(-1);
            result = result | partialImage;
            // masks.add(whiteMask);
        }
        if(config->segmentation.colorMasks["yellow"]){
            cv::Mat partialImage = image.clone();
            POBR::HSVMask yellowMask = POBR::MaskFactory(config->segmentation.yellowMask).build();
            yellowMask.apply(partialImage);
            partialImage = df.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = ef.filter(partialImage);
            partialImage = df.filter(partialImage);
            // cv::imshow("yellow", partialImage);
            // cv::waitKey(-1);
            result = result | partialImage;
            //masks.add(yellowMask);
        }
        // masks.apply(image);

        // image = df.filter(image);
        // image = ef.filter(image);
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
