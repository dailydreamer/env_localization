#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include "ipm.h"

bool comparePoints(const cv::Point p1, const cv::Point p2)
{
  if (p1.x != p2.x) {
    return p1.x < p2.x;
  } else {
    return p1.y < p2.y;
  }
}

//  in c++ 11, vector has move operator
std::vector<cv::Point> intersection(std::vector<cv::Point>& v1, std::vector<cv::Point>& v2)
{
  std::vector<cv::Point> res;
  std::sort(v1.begin(), v1.end(), comparePoints);
  std::sort(v2.begin(), v2.end(), comparePoints);
  std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(res), comparePoints);
  return res;
}

void vectorToMat(const std::vector<cv::Point>& points, cv::Mat& img, double height, double width) {
  img = cv::Mat::zeros(height, width, CV_8UC1);
  for (auto p : points) {
    img.at<int>(p) = 255;
  }
}

void drawCircle(const cv::Mat& img, const cv::Point& center, int radius = 10)
{
  int thickness = -1;
  int lineType = 8;
  cv::circle(img, center, radius, cv::Scalar(200, 200, 200), thickness);
}


void drawTrace(const cv::Mat& img, const std::vector<cv::Point>& trace) {
  for (auto p : trace) {
    drawCircle(img, p);
  }
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
    cv::namedWindow(windowLabels[i]+"origin", CV_WINDOW_NORMAL);
    cv::namedWindow(windowLabels[i]+"birdview", CV_WINDOW_NORMAL);
  }
  cv::namedWindow("intersection", CV_WINDOW_NORMAL);
  
  auto backgroundSubtractorPtr = cv::bgsegm::createBackgroundSubtractorMOG();
//    auto backgroundSubtractorPtr = cv::createBackgroundSubtractorMOG2(500, 16, true);
  
  double videoWidth = capture[0].get(CV_CAP_PROP_FRAME_WIDTH);
  double videoHeight = capture[0].get(CV_CAP_PROP_FRAME_HEIGHT);

  cv::Mat minimalBirdViewForegroundFrame;

  cv::Mat originFrame[cameraNum], foregroundFrame[cameraNum], birdViewForegroundFrame[cameraNum];
  cv::Mat birdViewFrame[cameraNum];
  std::vector<cv::Point> foregroundPoints[cameraNum];
  std::vector<cv::Point> birdViewForegroundPoints[cameraNum], minimalBirdViewForegroundPoints;
  
  const int historyTraceSize = 20;
  std::vector<cv::Point> trace(historyTraceSize);
  int trace_index = 0;
  
  // loop until press q or esc
  for (int key = 0; key != 'q' && key != 27; ) {
    for (int i = 0; i < cameraNum; i++) {
      if (!capture[i].read(originFrame[i])) {
	std::cerr << "ERROR: Cannot read next frame of camera " << i << std::endl;
	exit(EXIT_FAILURE);
      }
      backgroundSubtractorPtr -> apply(originFrame[i], foregroundFrame[i]);

//        cv::findNonZero(foregroundFrame[i], foregroundPoints[i]);
      ipm[i].getIpmImage(originFrame[i], birdViewFrame[i]);
      ipm[i].getIpmImage(foregroundFrame[i], birdViewForegroundFrame[i]);
//      cv::findNonZero(birdViewForegroundFrame[i], birdViewForegroundPoints[i]);
//        auto position = cv::mean(birdViewForegroundPoints[i]);
      
//	backgroundSubtractorPtr -> apply(birdViewFrame[i], foregroundFrame[i]);
//	cv::findNonZero(foregroundFrame[i], foregroundPoints[i]);
//	auto position = cv::mean(foregroundPoints[i]);
//	std::cout << foregroundPoints[i].size() << std::endl;
      
//        auto x = position[0], y = position[1];
//        drawCircle(birdViewFrame[i], cv::Point(x,y));
      cv::imshow(windowLabels[i]+"origin", birdViewForegroundFrame[i]);
//        cv::imshow(windowLabels[i]+"birdview", birdViewFrame[i]);
    }
    
    // TODO can be change to reduce format, so that can handle any number of cameras
//    auto minimalBirdViewForegroundPoints = intersection(birdViewForegroundPoints[0], birdViewForegroundPoints[1]);
//    vectorToMat(minimalBirdViewForegroundPoints, minimalBirdViewForegroundFrame, videoHeight, videoWidth);
    cv::bitwise_and(birdViewForegroundFrame[0], birdViewForegroundFrame[1], minimalBirdViewForegroundFrame);
    cv::findNonZero(minimalBirdViewForegroundFrame, minimalBirdViewForegroundPoints);
   // std::cout << minimalBirdViewForegroundFrame << std::endl;
   
    auto position = cv::mean(minimalBirdViewForegroundPoints);
    auto x = position[0], y = position[1];
    std::cout << "position: " <<  x << " " << y << std::endl;
    
    trace[trace_index % historyTraceSize] = cv::Point(x,y);
    trace_index++;
    
    cv::imshow("intersection", minimalBirdViewForegroundFrame);
    drawTrace(birdViewFrame[0], trace);
    cv::imshow(windowLabels[0]+"birdview",  birdViewFrame[0]);
    drawTrace(birdViewFrame[1], trace);
    cv::imshow(windowLabels[1]+"birdview",  birdViewFrame[1]);
    
    // TODO compupte theta with arctan of successive locations
    
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
