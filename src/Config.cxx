#include"Config.hxx"

AppConfig::AppConfig(const std::string configPath){
    const YAML::Node config = YAML::LoadFile(configPath);

    const YAML::Node preprocessingConfig = config["preprocessing"];
    const YAML::Node segmentationConfig = config["segmentation"];
    const YAML::Node identificationConfig = config["identification"];

    preprocessing = extractPreprocessingParams(preprocessingConfig);
    segmentation = extractSegmentationParams(segmentationConfig);
    identification = extractIdentificationParams(identificationConfig);
}

PreprocessingConfig AppConfig::extractPreprocessingParams(const YAML::Node node){
    PreprocessingConfig config;
    const YAML::Node prescaler = node["prescale"];
    config.size = std::make_pair<int, int>(prescaler["imageWidth"].as<int>(1200), prescaler["imageHeight"].as<int>(900));
    config.bicubic = prescaler["bicubicRatio"].as<double>(-0.75);
    const std::string algorithmName = prescaler["algorithm"].as<std::string>("Bicubic");
    std::map<std::string, AlgorithmType> algorithms = {
        { "NearestNeighbour", AlgorithmType::NearestNeighbour },
        { "Bilinear", AlgorithmType::Bilinear },
        { "Bicubic", AlgorithmType::Bicubic }
    };
    config.algorithm = algorithms[algorithmName];

    const YAML::Node operations = node["operations"];
    config.equalizeHistogram = operations["equalizeHistogram"].as<bool>(false);
    config.reduceColors = operations["reduceColors"].as<bool>(false);
    config.colorReducerRatio = operations["colorReducerRatio"].as<int>(10);

    return config;
}

SegmentationConfig AppConfig::extractSegmentationParams(const YAML::Node node){
    SegmentationConfig config;
    config.minimalSegmentSize = node["minimalSegmentSize"].as<int>(100);
    config.maskImage = node["maskImage"].as<bool>(false);
    config.colorMasks["red"] = node["useRed"].as<bool>(false);
    config.colorMasks["blue"] = node["useBlue"].as<bool>(false);
    config.colorMasks["white"] = node["useWhite"].as<bool>(false);
    config.colorMasks["yellow"] = node["useYellow"].as<bool>(false);

    if(config.colorMasks["red"]){
        const YAML::Node redNode = node["redMask"];
        config.extractParams(redNode, config.redMask);
    }
    
    if(config.colorMasks["blue"]){
        const YAML::Node blueNode = node["blueMask"];
        config.extractParams(blueNode, config.blueMask);
    }

    if(config.colorMasks["white"]){
        const YAML::Node whiteNode = node["whiteMask"];
        config.extractParams(whiteNode, config.whiteMask);
    }

    if(config.colorMasks["yellow"]){
        const YAML::Node yellowNode = node["yellowMask"];
        config.extractParams(yellowNode, config.yellowMask);
    }

    return config;
}

IdentificationConfig AppConfig::extractIdentificationParams(const YAML::Node node){
    IdentificationConfig config;

    config.minimalBunsWHRatio = node["minimalBunsWHRatio"].as<double>(0.8);
    config.maximalBunsWHRatio = node["maximalBunsWHRatio"].as<double>(1.3);
    config.bunsHeightToDistanceRatio = node["bunsHeightToDistanceRatio"].as<double>(3.5);

    config.detailPointsThreshold = node["detailPointsThreshold"].as<double>(1.0);
    config.letterCoefficient = node["letterCoefficient"].as<double>(0.1);
    config.stripeCoefficient = node["stripeCoefficient"].as<double>(0.3);

    return config;
}

void SegmentationConfig::extractParams(const YAML::Node node, std::map<std::string, int>& map){
    map["lowerHue"] = node["lowerHue"].as<int>(0);
    map["upperHue"] = node["upperHue"].as<int>(360);
    map["lowerSaturation"] = node["lowerSaturation"].as<int>(0);
    map["upperSaturation"] = node["upperSaturation"].as<int>(255);
    map["lowerValue"] = node["lowerValue"].as<int>(0);
    map["upperValue"] = node["upperValue"].as<int>(255);
}