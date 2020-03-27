#include<opencv2/opencv.hpp>
#include"POBR/Filters.hxx"

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Usage: Filters <imagePath> " << std::endl;
	    return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	    return -1;
    }

    POBR::MedianFilter mf(3);
    POBR::DilationFilter df(3);
    POBR::ErosionFilter ef(3);

    cv::imshow("original", image);
    cv::Mat medianFiltered = mf.filter(image);
    cv::imshow("median filtered", medianFiltered);

    cv::Mat dilationFiltered = df.filter(image);
    cv::imshow("dilation filtered", dilationFiltered);

    cv::Mat erosionFiltered = ef.filter(image);
    cv::imshow("erosion filtered", erosionFiltered);
    cv::waitKey(-1);

    cv::Mat lowPassKernel = (cv::Mat_<double>(5,5) << 0, 0, 0, 0, 0, 
                                                      0, 0.1, 0.1, 0.1, 0,
                                                      0, 0.1, 0.2, 0.1, 0,
                                                      0, 0.1, 0.1, 0.1, 0,
                                                      0, 0, 0, 0, 0);

    cv::Mat highPassKernel = (cv::Mat_<double>(5,5) << 0, 0, 0, 0, 0, 
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, -0.5, 5, -0.5, 0,
                                                      0, -0.5, -0.5, -0.5, 0,
                                                      0, 0, 0, 0, 0);

    POBR::ConvolutionalFilter lowPass(lowPassKernel);
    POBR::ConvolutionalFilter highPass(highPassKernel);

    cv::Mat lowPassImage = image.clone(), highPassImage = image.clone();

    lowPassImage = lowPass.filter(lowPassImage);
    highPassImage = highPass.filter(highPassImage);

    cv::imshow("original", image);
    cv::imshow("low pass", lowPassImage);
    cv::imshow("high pass", highPassImage);

    cv::imwrite("lowpass.png", lowPassImage);
    cv::imwrite("highpass.png", highPassImage);
    cv::waitKey(-1);
    return 0;
}