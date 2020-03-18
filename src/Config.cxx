#include"Config.hxx"

AppConfig::AppConfig(const std::string configPath){
    const YAML::Node config = YAML::LoadFile(configPath);

    reduceColors = config["reduceColors"].as<bool>(false);
    equalizeHistogram = config["equalizeHistogram"].as<bool>(false);
    maskImage = config["maskImage"].as<bool>(false);
    colorReducerRatio = config["colorReducerRatio"].as<int>(10);

    colorMasks["red"] = config["useRed"].as<bool>(false);
    colorMasks["blue"] = config["useBlue"].as<bool>(false);
    colorMasks["white"] = config["useWhite"].as<bool>(false);
    colorMasks["yellow"] = config["useYellow"].as<bool>(false);

    if(colorMasks["red"]){
        const YAML::Node redNode = config["redMask"];
        extractParams(redNode, redMask);
    }
    
    if(colorMasks["blue"]){
        const YAML::Node blueNode = config["blueMask"];
        extractParams(blueNode, blueMask);
    }

    if(colorMasks["white"]){
        const YAML::Node whiteNode = config["whiteMask"];
        extractParams(whiteNode, whiteMask);
    }

    if(colorMasks["yellow"]){
        const YAML::Node yellowNode = config["yellowMask"];
        extractParams(yellowNode, yellowMask);
    }
}

void AppConfig::extractParams(const YAML::Node node, std::map<std::string, int>& map){
    map["lowerHue"] = node["lowerHue"].as<int>(0);
    map["upperHue"] = node["upperHue"].as<int>(360);
    map["lowerSaturation"] = node["lowerSaturation"].as<int>(0);
    map["upperSaturation"] = node["upperSaturation"].as<int>(255);
    map["lowerValue"] = node["lowerValue"].as<int>(0);
    map["upperValue"] = node["upperValue"].as<int>(255);
}