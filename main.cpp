#include <string>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include "ipm.h"

void subtractBackground(std::string videoPath, bool debug = false) {
    cv::namedWindow("frame");
    cv::namedWindow("foreground");

    auto backgroundSubtractorPtr = cv::bgsegm::createBackgroundSubtractorGMG(10, 0.9);

    cv::VideoCapture capture(videoPath);
    if (!capture.isOpened()) {
        std::cerr << "Cannot open video file: " << videoPath << std::endl;
        exit(EXIT_FAILURE);
    }

    cv::Mat frame, foreground;
    for (int key = 0; key != 'q' && key != 27; ) {
        if (!capture.read(frame)) {
            std::cerr << "Cannot read next frame" << std::endl;
            exit(EXIT_FAILURE);
        }
        backgroundSubtractorPtr -> apply(frame, foreground);
        cv::imshow("frame", frame);
        cv::imshow("foreground", foreground);
        if (debug) {
            key = cv::waitKey(500);
        } else {
            key = cv::waitKey(100);
        }
    }

    cv::destroyAllWindows();
}

int main() {
    /*
    // The 4-points at the input image, camera coordinate
    std::vector<cv::Point2f> inputPoints;
    inputPoints.push_back( cv::Point2f(0, height) );
    inputPoints.push_back( cv::Point2f(width, height) );
    inputPoints.push_back( cv::Point2f(width/2+30, 140) );
    inputPoints.push_back( cv::Point2f(width/2-50, 140) );

    // The 4-points correspondences in the output image, world coordinate
    std::vector<cv::Point2f> outputPoints;
    outputPoints.push_back( cv::Point2f(0, height) );
    outputPoints.push_back( cv::Point2f(width, height) );
    outputPoints.push_back( cv::Point2f(width, 0) );
    outputPoints.push_back( cv::Point2f(0, 0) );

    // IPM object
    Ipm ipm( cv::Size(width, height), cv::Size(width, height), inputPoints, outputPoints);
    */

    std::string videoPath = "./Video_001.avi";
    subtractBackground(videoPath, false);
    return 0;
}