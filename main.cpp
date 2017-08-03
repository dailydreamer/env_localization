#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include "ipm.h"

int main() {

    bool debug = false;
    std::string cameraId = "0";
    std::string videoPath = "./camera1.avi";
    cv::VideoCapture capture(videoPath);
    if (!capture.isOpened()) {
        std::cerr << "Cannot open video file: " << videoPath << std::endl;
        exit(EXIT_FAILURE);
    }

    auto backgroundSubtractorPtr = cv::bgsegm::createBackgroundSubtractorMOG();
//    auto backgroundSubtractorPtr = cv::createBackgroundSubtractorMOG2(500, 16, true);
//    Ipm ipm("./ipm"+cameraId+".yml");
    cv::namedWindow("frame");
    cv::namedWindow("foreground");
    cv::Mat frame, foreground;
    std::vector<cv::Point> foregroundPoints;
    std::vector<cv::Point2d> birdViewForegroundPoints;
    for (int key = 0; key != 'q' && key != 27; ) {
        if (!capture.read(frame)) {
            std::cerr << "Cannot read next frame" << std::endl;
            exit(EXIT_FAILURE);
        }
        backgroundSubtractorPtr -> apply(frame, foreground);

        cv::findNonZero(foreground, foregroundPoints);
//        ipm.getIpmPoints(foregroundPoints, birdViewForegroundPoints);
        auto position = cv::mean(foregroundPoints);
        auto x = position[0], y = position[1];

//        std::cout << foregroundPoints << std::endl << x << " " << y << std::endl;

        cv::imshow("frame", frame);
        cv::imshow("foreground", foreground);
        if (debug) {
            key = cv::waitKey(500);
        } else {
            key = cv::waitKey(100);
        }
    }

//    std::string cameraId = "0";
//
//    cv::Mat inputImage = cv::imread("./extrinsic"+cameraId+".png");
//    Ipm ipm("./ipm"+cameraId+".yml");
//
//    cv::Mat birdViewImage;
//   // cv::cvtColor(inputImage, inputImage, CV_BGR2GRAY);
//    cv::namedWindow("input");
//    cv::imshow("input", inputImage);
//
//    ipm.getIpmImage(inputImage, birdViewImage);
//    cv::namedWindow("birdview");
//    cv::imshow("birdview", birdViewImage);
//
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}