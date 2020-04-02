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

    cv::Mat sobelKernel = (cv::Mat_<double>(5,5) << -0, -2, 0, 
                                                    -2, 8, -2,
                                                    0, -2, 0);
    POBR::ConvolutionalFilter s(sobelKernel);
    cv::Mat sobelFiltered = s.filter(image);
    cv::imshow("sobel filtered", sobelFiltered);
    cv::waitKey(-1);

    POBR::MedianFilter mf(3);
    cv::Mat medianFiltered = mf.filter(image);
    cv::imshow("median filtered", medianFiltered);

    POBR::GaussianFilter gf(11, 1.0);
    cv::Mat gaussianFiltered = gf.filter(image);
    cv::imshow("gaussian filtered", gaussianFiltered);

    POBR::DilationFilter df(3);
    cv::Mat dilationFiltered = df.filter(image);
    cv::imshow("dilation filtered", dilationFiltered);

    POBR::ErosionFilter ef(3);
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