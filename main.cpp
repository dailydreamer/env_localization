#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include "ipm.h"

void drawCircle( cv::Mat img, cv::Point center )
{
    int thickness = -1;
    int lineType = 8;
    int radius = 20;
    cv::circle(img, center, radius, cv::Scalar(200, 200, 200), thickness);
}


int main() {

    bool debug = false;
    const int cameraNum = 2;

    cv::VideoCapture capture[cameraNum];
    Ipm ipm[cameraNum];
    std::string windowLabels[cameraNum];
    for (int i = 0; i < cameraNum; i++) {
      capture[i].open("./camera" + std::to_string(i) + ".avi");
      if (!capture[i].isOpened()) {
        std::cerr << "ERROR: Cannot open video file of camera " << i << std::endl;
        exit(EXIT_FAILURE);
      }
      ipm[i].loadFromFile("./ipm" + std::to_string(i) + ".yml");
      windowLabels[i] = "camera" + std::to_string(i);
      cv::namedWindow(windowLabels[i]+"origin", CV_WINDOW_AUTOSIZE);
      cv::namedWindow(windowLabels[i]+"birdview", CV_WINDOW_AUTOSIZE);
    }
    
    auto backgroundSubtractorPtr = cv::bgsegm::createBackgroundSubtractorMOG();
//    auto backgroundSubtractorPtr = cv::createBackgroundSubtractorMOG2(500, 16, true);
    
    cv::Mat frame[cameraNum], foreground[cameraNum];
    cv::Mat birdViewFrame[cameraNum];
    std::vector<cv::Point> foregroundPoints[cameraNum];
    std::vector<cv::Point2d> birdViewForegroundPoints[cameraNum];
    // loop until press q or esc
    for (int key = 0; key != 'q' && key != 27; ) {
      for (int i = 0; i < cameraNum; i++) {
	if (!capture[i].read(frame[i])) {
	  std::cerr << "ERROR: Cannot read next frame of camera " << i << std::endl;
	  exit(EXIT_FAILURE);
        }
        backgroundSubtractorPtr -> apply(frame[i], foreground[i]);
        cv::findNonZero(foreground[i], foregroundPoints[i]);
        ipm[i].getIpmImage(frame[i], birdViewFrame[i]);
//        ipm.getIpmPoints(foregroundPoints, birdViewForegroundPoints);
        auto position = cv::mean(foregroundPoints[i]);
        auto x = position[0], y = position[1];

//        std::cout << foregroundPoints << std::endl << x << " " << y << std::endl;

        drawCircle(foreground[i], cv::Point(x,y));
        cv::imshow(windowLabels[i]+"origin", frame[i]);
        cv::imshow(windowLabels[i]+"birdview", birdViewFrame[i]);
      }
      
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
