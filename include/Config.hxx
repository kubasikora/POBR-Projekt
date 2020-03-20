#include<yaml-cpp/yaml.h>

struct AppConfig {
    AppConfig(const std::string configPath);

    std::map<std::string, int> redMask;
    std::map<std::string, int> blueMask;
    std::map<std::string, int> whiteMask;
    std::map<std::string, int> yellowMask;
    std::map<std::string, bool> colorMasks;
    int colorReducerRatio;
    bool reduceColors;
    bool equalizeHistogram;
    bool maskImage;
    std::pair<int, int> imageSize;

  private:
    void extractParams(const YAML::Node node, std::map<std::string, int>& map);
};