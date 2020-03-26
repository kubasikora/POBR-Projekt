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

    const int width = 1000, height = 1000;
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

    std::vector<std::pair<int, int>> sizes;
    sizes.push_back(std::make_pair(50,50));
    sizes.push_back(std::make_pair(100,100));
    sizes.push_back(std::make_pair(200,200));
    sizes.push_back(std::make_pair(400,400));
    sizes.push_back(std::make_pair(800,800));
    sizes.push_back(std::make_pair(1600,1600));
    sizes.push_back(std::make_pair(3200,3200));
    sizes.push_back(std::make_pair(6400,6400));

    std::for_each(sizes.begin(), sizes.end(), [&](auto size){
        std::cout << size.first << ", ";
        cv::Mat nn = image.clone();
        POBR::NearestNeighbourInterpolationResizer nnr(size);
        double tnn = (double)cv::getTickCount();
        nn = nnr.resize(nn);
        tnn = ((double)cv::getTickCount() - tnn) / cv::getTickFrequency();
         std::cout << "NN: " << tnn << "ms ";
         
        cv::Mat bl = image.clone();
        POBR::BilinearInterpolationResizer blr(size);
        double tbl = (double)cv::getTickCount();
        bl = blr.resize(bl);
        tbl = ((double)cv::getTickCount() - tbl) / cv::getTickFrequency();
         std::cout << "BL: " << tbl << "ms ";

        cv::Mat bc = image.clone();
        POBR::BicubicInterpolationResizer blc(size.first, size.second);
        double tbc = (double)cv::getTickCount();
        bc = blc.resize(bc);
        tbc = ((double)cv::getTickCount() - tbc) / cv::getTickFrequency();
        std::cout << "BC: " << tbc << "ms ";
        std::cout << std::endl;
    });
    return 0;

}