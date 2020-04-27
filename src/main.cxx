#include<opencv2/opencv.hpp>
#include<yaml-cpp/yaml.h>
#include"Config.hxx"
#include"POBR/Segmentation.hxx"
#include"POBR/Masks.hxx"
#include"POBR/Filters.hxx"
#include"POBR/Preprocessing.hxx"
#include"POBR/Identification.hxx"

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

    /* preprocessing - prescaling */
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

    cv::Mat ogImage = image.clone(); 

    /* preprocessing - reduction of colors */
    if(config->preprocessing.reduceColors) {
        POBR::ColorReducer reducer(config->preprocessing.colorReducerRatio);
        reducer.reduce(image);
    }
    
    /* preprocessing - gaussian filtering */
    POBR::GaussianFilter gf(config->preprocessing.gaussianRadius, config->preprocessing.sigma); 
    image = gf.filter(image);

    /* preprocessing - high pass filtering */
    cv::Mat highPassKernel = (cv::Mat_<double>(3,3) << -0.5, -0.5, -0.5,
                                                       -0.5,   5,  -0.5,
                                                       -0.5, -0.5, -0.5);
    POBR::ConvolutionalFilter highPass(highPassKernel);
    image = highPass.filter(image);

    /* conversion to hsv colour space */
    POBR::BGR2HSVConverter converter;
    converter.convert(image);

    /* preprocessing - histogram equalization */
    if(config->preprocessing.equalizeHistogram) {
        POBR::HistogramEqualizer equalizer;
        equalizer.equalize(image);
    }

    /* mask image by color thresholding */
    std::vector<std::pair<POBR::Color, cv::Mat>> images;
    if(config->segmentation.maskImage){
        POBR::MaskApplier masks;
        if(config->segmentation.colorMasks["red"]){
            cv::Mat redImage = image.clone();
            POBR::HSVMask redMask = POBR::MaskFactory(config->segmentation.redMask).build();
            redImage = redMask.apply(redImage);

            POBR::DilationFilter df(3);
            redImage = df.filter(redImage);  
              
            images.push_back(std::make_pair(POBR::Color::RED, redImage)); 
        }
        if(config->segmentation.colorMasks["blue"]){
            cv::Mat blueImage = image.clone();
            POBR::HSVMask blueMask = POBR::MaskFactory(config->segmentation.blueMask).build();
            blueImage = blueMask.apply(blueImage);

            images.push_back(std::make_pair(POBR::Color::BLUE, blueImage)); 
        }
        if(config->segmentation.colorMasks["white"]){
            cv::Mat whiteImage = image.clone();
            POBR::HSVMask whiteMask = POBR::MaskFactory(config->segmentation.whiteMask).build();
            whiteImage = whiteMask.apply(whiteImage);

            images.push_back(std::make_pair(POBR::Color::WHITE, whiteImage)); 
        }
        if(config->segmentation.colorMasks["yellow"]){
            cv::Mat yellowImage = image.clone();
            POBR::HSVMask yellowMask = POBR::MaskFactory(config->segmentation.yellowMask).build();
            yellowImage = yellowMask.apply(yellowImage);

            POBR::ErosionFilter ef(3); POBR::DilationFilter df(5);
            yellowImage = df.filter(yellowImage); yellowImage = ef.filter(yellowImage);

            images.push_back(std::make_pair(POBR::Color::YELLOW, yellowImage));
        }
    }
    
    /* merge masked image into one matrix */
    POBR::ImageMerger im;
    cv::Mat_<POBR::Color> maskedImage = im.mergeImage(images);

    /* segment image */
    POBR::SegmentationUnit su(config->segmentation.minimalSegmentSize);
    auto descriptors = su.segmentImage(maskedImage);
    std::cout << "Extracted segments: " << descriptors.size() << std::endl;
    std::sort(descriptors.begin(), descriptors.end(), [](auto& v1, auto& v2){
        return v1.getArea() > v2.getArea();
    });

    /* BK logo model */
    const std::array<double, 5> whiteModel = { 0.009737395, 2.18181e-7, 1.48058e-7, 4.23793e-15, 8.50314e-10 };
    const std::array<double, 5> blueModel = {0.5879375, 0.000248822, 5.08791e-5, 5.762582e-10, -1.1759715e-5};
    const std::array<double, 5> lowerBunModel = {0.766357, 3.059285e-5, 2.84446e-6, -3.87055e-12, -6.78627e-7};
    const std::array<double, 5> upperBunModel = {0.7692, 5.03532e-5, 4.84897e-6, -2.43564e-11, -2.41654e-6};
    const double whiteWHDif = 0.9571055, whiteWHThreshold = 0.1867405*2;
    const double blueWHDif = 2.15142, blueWHThreshold = 0.748557*2;
    const double yellowWHDif = 2.31335, yellowWHThreshold = 0.57327*2;

    /* load max diffs */
    const double whiteMomentDif = config->identification.whiteMaxDifference;
    const double blueMomentDif = config->identification.blueMaxDifference;
    const double yellowMomentDif = config->identification.yellowMaxDifference;

    /* filter out segments based on hu's invariant moments and divide into color bins */
    std::map<POBR::Color, std::vector<POBR::SegmentDescriptor>> bins = {
        {POBR::Color::RED, {} },
        {POBR::Color::BLUE, {} },
        {POBR::Color::WHITE, {} },
        {POBR::Color::YELLOW, {} },
    };

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

            if((difLo > yellowMomentDif && difUp > yellowMomentDif) || std::abs(yellowWHDif - segment.getwhRatio()) > yellowWHThreshold)
                break;

            bins[POBR::Color::YELLOW].push_back(segment); break;

          default:
            break;
        };
    });

    /* find pairs of buns */
    std::vector<POBR::SegmentDescriptor> pairs; 
    for(unsigned i = 0; i < bins[POBR::Color::YELLOW].size(); ++i){
        for(unsigned j = i+1; j < bins[POBR::Color::YELLOW].size(); ++j){
            auto segment1 = bins[POBR::Color::YELLOW][i], segment2 = bins[POBR::Color::YELLOW][j];

            const auto s1Cog = segment1.getCenterOfGravity(), s2Cog = segment2.getCenterOfGravity();
            const double distance = std::sqrt(std::pow(s1Cog.first - s2Cog.first, 2) + std::pow(s1Cog.second - s2Cog.second, 2));
            const double maxHeight = std::max(segment1.getBoundingBox().height, segment2.getBoundingBox().height);
            const double ratio = segment1.getwhRatio() / segment2.getwhRatio();

            if(ratio > config->identification.minimalBunsWHRatio 
            && ratio < config->identification.maximalBunsWHRatio
            && distance < config->identification.bunsHeightToDistanceRatio*maxHeight){
                pairs.push_back(segment1 + segment2);
            }
        }
    }
    std::cout << "Found " << pairs.size() << " pairs of buns" << std::endl;  

    /* filter based on white backgrounds */
    std::vector<POBR::SegmentDescriptor> bunsWithBackgrounds; 
    std::for_each(pairs.begin(), pairs.end(), [&](auto& buns){
        const auto bunsCoG = buns.getCenterOfGravity(); 
        std::vector<POBR::SegmentDescriptor> possibleBackgrounds;
        std::for_each(bins[POBR::Color::WHITE].begin(), bins[POBR::Color::WHITE].end(), [&](auto& background){
            const POBR::BoundingBox bb = background.getBoundingBox();
            if(bunsCoG.first > bb.y && bunsCoG.first < bb.y + bb.height && bunsCoG.second > bb.x && bunsCoG.second < bb.x + bb.width)
                possibleBackgrounds.push_back(background);
        });
        if(!possibleBackgrounds.empty()){
            std::sort(possibleBackgrounds.begin(), possibleBackgrounds.end(), [](auto& v1, auto& v2){
                return v1.getArea() > v2.getArea();
            });
            bunsWithBackgrounds.push_back(buns + possibleBackgrounds.front());
        }
    });
    std::cout << "Found " << bunsWithBackgrounds.size() << " pairs of buns with appropiate background" << std::endl;

    /* filter based on red letters and blue stripes */
    std::vector<POBR::SegmentDescriptor> logos;
    std::for_each(bunsWithBackgrounds.begin(), bunsWithBackgrounds.end(), [&](auto& buns){
        int lettersCount = 0;
        std::for_each(bins[POBR::Color::RED].begin(), bins[POBR::Color::RED].end(), [&](auto& letter){
            const auto letterCoG =  letter.getCenterOfGravity();
            const POBR::BoundingBox bb = buns.getBoundingBox();
            if(letterCoG.first > bb.y && letterCoG.first < bb.y + bb.height && letterCoG.second > bb.x && letterCoG.second)
                ++lettersCount;
        });
        int stripesCount = 0;
        std::for_each(bins[POBR::Color::BLUE].begin(), bins[POBR::Color::BLUE].end(), [&](auto& stripe){
            const auto stripeCoG =  stripe.getCenterOfGravity();
            const POBR::BoundingBox bb = buns.getBoundingBox();
            if(stripeCoG.first > bb.y && stripeCoG.first < bb.y + bb.height && stripeCoG.second > bb.x && stripeCoG.second)
                ++stripesCount;
        });

        const double pointsThreshold = config->identification.detailPointsThreshold;
        const double letterCoeff = config->identification.letterCoefficient;
        const double stripeCoeff = config->identification.stripeCoefficient;

        const double result = letterCoeff*lettersCount + stripeCoeff*stripesCount;

        if(result >= pointsThreshold){
            logos.push_back(buns);
        }
    });

    /* draw logos */
    std::for_each(logos.begin(), logos.end(), [&](auto& segment){
        POBR::BoundingBox bb = segment.getBoundingBox();
        if(bb.width == 0 || bb.height == 0)
            return;
        
        cv::Mat roi = ogImage(cv::Rect(bb.x, bb.y, bb.width, bb.height));
        POBR::drawSegmentBoundary(ogImage, bb);
    });

    std::cout << "Number of logos found: " << logos.size() << std::endl;
    
    cv::imshow("Picture", ogImage);
    cv::imwrite("resultbk.jpg", ogImage);
    cv::waitKey(-1);
    return logos.size();
}
