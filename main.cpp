#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include "ipm.h"

bool comparePoints(const cv::Point2d p1, const cv::Point2d p2)
{
  if (p1.x != p2.x) {
    return p1.x < p2.x;
  } else {
    return p1.y < p2.y;
  }
}

//  in c++ 11, vector has move operator
std::vector<cv::Point2d> intersection(std::vector<cv::Point2d>& v1, std::vector<cv::Point2d>& v2)
{
    std::vector<cv::Point2d> res;
    std::sort(v1.begin(), v1.end(), comparePoints);
    std::sort(v2.begin(), v2.end(), comparePoints);
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(res), comparePoints);
    return res;
}

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
    
    cv::Mat originFrame[cameraNum], foregroundFrame[cameraNum];
    cv::Mat birdViewFrame[cameraNum];
    std::vector<cv::Point> foregroundPoints[cameraNum];
    std::vector<cv::Point2d> birdViewForegroundPoints[cameraNum];
    // loop until press q or esc
    for (int key = 0; key != 'q' && key != 27; ) {
      for (int i = 0; i < cameraNum; i++) {
	if (!capture[i].read(originFrame[i])) {
	  std::cerr << "ERROR: Cannot read next frame of camera " << i << std::endl;
	  exit(EXIT_FAILURE);
        }
        backgroundSubtractorPtr -> apply(originFrame[i], foregroundFrame[i]);
        cv::findNonZero(foregroundFrame[i], foregroundPoints[i]);
        ipm[i].getIpmImage(originFrame[i], birdViewFrame[i]);
        ipm[i].getIpmPoints(foregroundPoints[i], birdViewForegroundPoints[i]);
        auto position = cv::mean(birdViewForegroundPoints[i]);
        auto x = position[0], y = position[1];
       drawCircle(birdViewFrame[i], cv::Point(x,y));
        cv::imshow(windowLabels[i]+"origin", originFrame[i]);
        cv::imshow(windowLabels[i]+"birdview", birdViewFrame[i]);
      }
      
      // TODO can be change to reduce format, so that can handle any number of cameras
//      auto minimalBirdViewForegroundPoints = intersection(birdViewForegroundPoints[0], birdViewForegroundPoints[1]);
//      auto position = cv::mean(minimalBirdViewForegroundPoints);
//      auto x = position[0], y = position[1];
//      std::cout << "position: " <<  x << " " << y << std::endl;
//      drawCircle(birdViewFrame[0], cv::Point(x,y));
//      cv::imshow(windowLabels[0]+"birdview",  birdViewFrame[0]);
//      drawCircle(birdViewFrame[1], cv::Point(x,y));
//      cv::imshow(windowLabels[1]+"birdview",  birdViewFrame[1]);
  
      if (debug) {
	key = cv::waitKey(500);
      } else {
	key = cv::waitKey(100);
      }
    }

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
