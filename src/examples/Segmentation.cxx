// #include<opencv2/opencv.hpp>
// #include"POBR/Segmentation.hxx"
// #include"POBR/Masks.hxx"
// #include"POBR/Filters.hxx"
// #include"POBR/Preprocessing.hxx"

// int main(int argc, char** argv){
//     if(argc < 2){
//         std::cout << "Usage: Segmentation <imagePath> " << std::endl;
// 	    return -1;
//     }

//     cv::Mat image;
//     image = cv::imread(argv[1], cv::IMREAD_COLOR);
//     if(!image.data){
//         std::cout << "Empty image" << std::endl;
// 	    return -1;
//     };

//     POBR::BicubicInterpolationResizer bcr(1000, 1000);
//     image = bcr.resize(image);

//     POBR::GaussianFilter gf(5, 1.0);
//     image = gf.filter(image);

//     cv::imshow("filtered", image);
//     cv::waitKey(-1);

//     cv::Mat highPassKernel = (cv::Mat_<double>(5,5) << 0, 0, 0, 0, 0, 
//                                                       0, -0.5, -0.5, -0.5, 0,
//                                                       0, -0.5, 5, -0.5, 0,
//                                                       0, -0.5, -0.5, -0.5, 0,
//                                                       0, 0, 0, 0, 0);
//     POBR::ConvolutionalFilter highPass(highPassKernel);
//     image = highPass.filter(image);
    
//     POBR::BGR2HSVConverter converter;
//     converter.convert(image);

//     POBR::ErosionFilter ef(3);
//     POBR::DilationFilter df(3);
//     POBR::DilationFilter dfl(5);

//     POBR::HSVMask redMask(POBR::HueInterval(-20, 20), POBR::SaturationInterval(180, 255), POBR::ValueInterval(100, 255));
//     cv::Mat x = redMask.apply(image);
//     std::cout << "x filtered" << std::endl;
//     cv::Mat xef  = ef.filter(x);
//     std::cout << "xef filtered" << std::endl;
//     cv::Mat xdf = df.filter(xef);
//     std::cout << "xdf filtered" << std::endl;
//     cv::Mat xddf  = df.filter(xdf);
//     std::cout << "xef filtered" << std::endl;

//     POBR::HSVMask whiteMask(POBR::HueInterval(0, 360), POBR::SaturationInterval(0, 40), POBR::ValueInterval(100, 255));
//     cv::Mat y = whiteMask.apply(image);
//     std::cout << "y filtered" << std::endl;
//     cv::Mat yef = ef.filter(y);
//     std::cout << "yef filtered" << std::endl;
//     cv::Mat ydf = df.filter(yef);
//     std::cout << "ydf filtered" << std::endl;
//     cv::Mat yddf = df.filter(ydf);
//     std::cout << "yef filtered" << std::endl;

//     POBR::HSVMask yellowMask(POBR::HueInterval(20, 60), POBR::SaturationInterval(70, 255), POBR::ValueInterval(150, 255));
//     cv::Mat z = yellowMask.apply(image);
//     std::cout << "z filtered" << std::endl;
//     cv::Mat zef = ef.filter(z);
//     std::cout << "zef filtered" << std::endl;
//     cv::Mat zdf = df.filter(zef);
//     std::cout << "zdf filtered" << std::endl;
//     cv::Mat zddf = df.filter(zdf);
//     std::cout << "zef filtered" << std::endl;

//     POBR::HSVMask blueMask(POBR::HueInterval(220, 260), POBR::SaturationInterval(100, 255), POBR::ValueInterval(0, 255));
//     cv::Mat w = blueMask.apply(image);
//     std::cout << "w filtered" << std::endl;
//     cv::Mat wef = ef.filter(w);
//     std::cout << "wef filtered" << std::endl;
//     cv::Mat wddf = dfl.filter(wef);
//     std::cout << "wef filtered" << std::endl;
//     cv::Mat wede = ef.filter(wddf);
//     std::cout << "wef filtered" << std::endl;

//     cv::imshow("red", xddf);
//     cv::imshow("white", yddf);
//     cv::imshow("yellow", zddf);
//     cv::imshow("blue", wede);
//     cv::waitKey(-1);
//     return 0;
// }

// #include "opencv2/highgui/highgui.hpp"
// #include "opencv2/imgproc/imgproc.hpp"
// #include <iostream>
// #include <stdio.h>

// using namespace cv;
// using namespace std;

// /** @function main */
// int main(int argc, char** argv)
// {
//   Mat src, src_gray;

//   /// Read the image
//   src = imread( argv[1], 1 );

//   if( !src.data )
//     { return -1; }

//   /// Convert it to gray
//   cvtColor( src, src_gray, CV_BGR2GRAY );

//   /// Reduce the noise so we avoid false circle detection
//   GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

//   vector<Vec3f> circles;

//   /// Apply the Hough Transform to find the circles
//   HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 0, 0 );

//   /// Draw the circles detected
//   for( size_t i = 0; i < circles.size(); i++ )
//   {
//       Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//       int radius = cvRound(circles[i][2]);
//       // circle center
//       circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
//       // circle outline
//       circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
//    }

//   /// Show your results
//   namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
//   imshow( "Hough Circle Transform Demo", src );

//   waitKey(0);
//   return 0;
// }

/**
 * @file HoughCircle_Demo.cpp
 * @brief Demo code for Hough Transform
 * @author OpenCV team
 */

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

namespace
{
    // windows and trackbars name
    const std::string windowName = "Hough Circle Detection Demo";
    const std::string cannyThresholdTrackbarName = "Canny threshold";
    const std::string accumulatorThresholdTrackbarName = "Accumulator Threshold";

    // initial and max values of the parameters of interests.
    const int cannyThresholdInitialValue = 100;
    const int accumulatorThresholdInitialValue = 50;
    const int maxAccumulatorThreshold = 200;
    const int maxCannyThreshold = 255;

    void HoughDetection(const Mat& src_gray, const Mat& src_display, int cannyThreshold, int accumulatorThreshold)
    {
        // will hold the results of the detection
        std::vector<Vec3f> circles;
        // runs the actual detection
        HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, cannyThreshold, accumulatorThreshold, 0, 0 );

        // clone the colour, input image for displaying purposes
        Mat display = src_display.clone();
        for( size_t i = 0; i < circles.size(); i++ )
        {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle( display, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( display, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }

        // shows the results
        imshow( windowName, display);
    }
}


int main(int argc, char** argv)
{
    Mat src, src_gray;

    /// Read the image
    src = imread( argv[1], 1 );

    if( !src.data )
        { return -1; }

    if( src.empty() )
    {
        std::cerr << "Invalid input image\n";
        std::cout << "Usage : " << argv[0] << " <path_to_input_image>\n";;
        return -1;
    }

    // Convert it to gray
    cvtColor( src, src_gray, COLOR_BGR2GRAY );

    // Reduce the noise so we avoid false circle detection
    GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

    //declare and initialize both parameters that are subjects to change
    int cannyThreshold = cannyThresholdInitialValue;
    int accumulatorThreshold = accumulatorThresholdInitialValue;

    // create the main window, and attach the trackbars
    namedWindow( windowName, WINDOW_AUTOSIZE );
    createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold,maxCannyThreshold);
    createTrackbar(accumulatorThresholdTrackbarName, windowName, &accumulatorThreshold, maxAccumulatorThreshold);

    // infinite loop to display
    // and refresh the content of the output image
    // until the user presses q or Q
    char key = 0;
    while(key != 'q' && key != 'Q')
    {
        // those parameters cannot be =0
        // so we must check here
        cannyThreshold = std::max(cannyThreshold, 1);
        accumulatorThreshold = std::max(accumulatorThreshold, 1);

        //runs the detection, and update the display
        HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold);

        // get user key
        key = (char)waitKey(10);
    }

    return 0;
}