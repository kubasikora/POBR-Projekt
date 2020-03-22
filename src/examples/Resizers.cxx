#include<opencv2/opencv.hpp>
#include"POBR/Preprocessing.hxx"

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Usage: Resizers <imagePath> " << std::endl;
	    return -1;
    }

    cv::Mat image;
    image = cv::imread(argv[1], cv::IMREAD_COLOR);
    if(!image.data){
        std::cout << "Empty image" << std::endl;
	    return -1;
    }

    const int width = 100, height = 100;
    cv::Mat nn = image.clone(), bil = image.clone(), bic = image.clone();
    POBR::NearestNeighbourInterpolationResizer nnr(height, width);
    POBR::BilinearInterpolationResizer blr(height, width);
    POBR::BicubicInterpolationResizer bcr(height, width);

    nnr.resize(nn);
    blr.resize(bil);
    bcr.resize(bic);

    cv::namedWindow("nn", cv::WINDOW_AUTOSIZE);
    cv::imshow("nn", nn);

    cv::namedWindow("bilinear", cv::WINDOW_AUTOSIZE);
    cv::imshow("bilinear", bil);

    cv::namedWindow("bicubic", cv::WINDOW_AUTOSIZE);
    cv::imshow("bicubic", bic);

    cv::waitKey(0);

    cv::imwrite("nn.png", nn);
    cv::imwrite("bl.png", bil);
    cv::imwrite("bc.png", bic);
    return 0;

}