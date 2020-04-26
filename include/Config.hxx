#include<yaml-cpp/yaml.h>

enum AlgorithmType {
    NearestNeighbour,
    Bilinear,
    Bicubic
};

struct PreprocessingConfig {
  public:
    AlgorithmType algorithm;
    std::pair<int, int> size;
    double bicubic;
    bool equalizeHistogram;
    bool reduceColors;
    int colorReducerRatio;
    int gaussianRadius;
    double sigma;
};

struct SegmentationConfig {
  public:
    int minimalSegmentSize;
    bool maskImage;
    std::map<std::string, bool> colorMasks;

    std::map<std::string, int> redMask;
    std::map<std::string, int> blueMask;
    std::map<std::string, int> whiteMask;
    std::map<std::string, int> yellowMask;
  private:
    friend class AppConfig;
    void extractParams(const YAML::Node node, std::map<std::string, int>& map);
};

struct IdentificationConfig {
  public:
    double minimalBunsWHRatio;
    double maximalBunsWHRatio;
    double bunsHeightToDistanceRatio;

    double detailPointsThreshold;
    double letterCoefficient;
    double stripeCoefficient;
  private:
    friend class AppConfig;
};

struct AppConfig {
    AppConfig(const std::string configPath);

    PreprocessingConfig preprocessing;
    SegmentationConfig segmentation;
    IdentificationConfig identification;

  private:
    PreprocessingConfig extractPreprocessingParams(const YAML::Node node);
    SegmentationConfig extractSegmentationParams(const YAML::Node node);
    IdentificationConfig extractIdentificationParams(const YAML::Node node);
};