#include<opencv2/opencv.hpp>
#include"POBR/Segmentation.hxx"
#include"POBR/Masks.hxx"
#include"POBR/Filters.hxx"
#include"POBR/Preprocessing.hxx"

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

    cv::imshow("filtered", image);
    cv::waitKey(-1);

    cv::Mat highPassKernel = (cv::Mat_<double>(5,5) << 0, 0, 0, 0, 0, 
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, -0.5, 5, -0.5, 0,
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, 0, 0, 0, 0);
    POBR::ConvolutionalFilter highPass(highPassKernel);
    image = highPass.filter(image);
    
    POBR::BGR2HSVConverter converter;
    converter.convert(image);

    POBR::ErosionFilter ef(3);
    POBR::DilationFilter df(3);
    POBR::DilationFilter dfl(5);

    POBR::HSVMask redMask(POBR::HueInterval(-20, 20), POBR::SaturationInterval(180, 255), POBR::ValueInterval(100, 255));
    cv::Mat x = redMask.apply(image);
    std::cout << "x filtered" << std::endl;
    cv::Mat xef  = ef.filter(x);
    std::cout << "xef filtered" << std::endl;
    cv::Mat xdf = df.filter(xef);
    std::cout << "xdf filtered" << std::endl;
    cv::Mat xddf  = df.filter(xdf);
    std::cout << "xef filtered" << std::endl;

    POBR::HSVMask whiteMask(POBR::HueInterval(0, 360), POBR::SaturationInterval(0, 40), POBR::ValueInterval(100, 255));
    cv::Mat y = whiteMask.apply(image);
    std::cout << "y filtered" << std::endl;
    cv::Mat yef = ef.filter(y);
    std::cout << "yef filtered" << std::endl;
    cv::Mat ydf = df.filter(yef);
    std::cout << "ydf filtered" << std::endl;
    cv::Mat yddf = df.filter(ydf);
    std::cout << "yef filtered" << std::endl;

    POBR::HSVMask yellowMask(POBR::HueInterval(20, 60), POBR::SaturationInterval(70, 255), POBR::ValueInterval(150, 255));
    cv::Mat z = yellowMask.apply(image);
    std::cout << "z filtered" << std::endl;
    cv::Mat zef = ef.filter(z);
    std::cout << "zef filtered" << std::endl;
    cv::Mat zdf = df.filter(zef);
    std::cout << "zdf filtered" << std::endl;
    cv::Mat zddf = df.filter(zdf);
    std::cout << "zef filtered" << std::endl;

    POBR::HSVMask blueMask(POBR::HueInterval(220, 260), POBR::SaturationInterval(100, 255), POBR::ValueInterval(0, 255));
    cv::Mat w = blueMask.apply(image);
    std::cout << "w filtered" << std::endl;
    cv::Mat wef = ef.filter(w);
    std::cout << "wef filtered" << std::endl;
    cv::Mat wddf = dfl.filter(wef);
    std::cout << "wef filtered" << std::endl;
    cv::Mat wede = ef.filter(wddf);
    std::cout << "wef filtered" << std::endl;

    cv::imshow("red", xddf);
    cv::imshow("white", yddf);
    cv::imshow("yellow", zddf);
    cv::imshow("blue", wede);
    cv::waitKey(-1);
    return 0;
}