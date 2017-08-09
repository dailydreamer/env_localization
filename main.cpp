#include <iostream>
#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include "ipm.h"

const double PI = 3.14159265;

void drawCircle(const cv::Mat &img, const cv::Point &center, int radius = 10) {
  int thickness = -1;
  int lineType = 8;
  cv::circle(img, center, radius, cv::Scalar(200, 200, 200), thickness);
}

void drawTrace(const cv::Mat &img, const std::vector<cv::Point> &trace) {
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
  for (int i = 0; i < cameraNum; i++)
  {
    capture[i].open("./camera" + std::to_string(i) + ".avi");
    if (!capture[i].isOpened())
    {
      std::cerr << "ERROR: Cannot open video file of camera " << i << std::endl;
      exit(EXIT_FAILURE);
    }
    ipm[i].loadFromFile("./ipm" + std::to_string(i) + ".yml");
    windowLabels[i] = "camera" + std::to_string(i);
    cv::namedWindow(windowLabels[i] + "origin", CV_WINDOW_NORMAL);
    cv::namedWindow(windowLabels[i] + "birdview", CV_WINDOW_NORMAL);
  }
  cv::namedWindow("intersection", CV_WINDOW_NORMAL);

  auto backgroundSubtractorPtr = cv::bgsegm::createBackgroundSubtractorMOG();
  //    auto backgroundSubtractorPtr = cv::createBackgroundSubtractorMOG2(500, 16, true);

  double videoWidth = capture[0].get(CV_CAP_PROP_FRAME_WIDTH);
  double videoHeight = capture[0].get(CV_CAP_PROP_FRAME_HEIGHT);

  cv::Mat originFrame[cameraNum], foregroundFrame[cameraNum], birdViewForegroundFrame[cameraNum];
  cv::Mat birdViewFrame[cameraNum];
  cv::Mat intersectionBirdViewForegroundFrame;
  std::vector<cv::Point> intersectionBirdViewForegroundPoints;

  const int historyTraceSize = 20;
  std::vector<cv::Point> trace(historyTraceSize);
  cv::Point previousPoint(0, 0);
  int trace_index = 0;

  // loop until press q or esc
  for (int key = 0; key != 'q' && key != 27;) {
    for (int i = 0; i < cameraNum; i++) {
      if (!capture[i].read(originFrame[i])) {
        std::cerr << "ERROR: Cannot read next frame of camera " << i << std::endl;
        exit(EXIT_FAILURE);
      }
      backgroundSubtractorPtr->apply(originFrame[i], foregroundFrame[i]);

      ipm[i].getIpmImage(originFrame[i], birdViewFrame[i]);
      ipm[i].getIpmImage(foregroundFrame[i], birdViewForegroundFrame[i]);

      cv::imshow(windowLabels[i] + "origin", birdViewForegroundFrame[i]);
    }

    // TODO can be change to reduce format, so that can handle any number of cameras
    cv::bitwise_and(birdViewForegroundFrame[0], birdViewForegroundFrame[1], intersectionBirdViewForegroundFrame);
    cv::findNonZero(intersectionBirdViewForegroundFrame, intersectionBirdViewForegroundPoints);

    auto position = cv::mean(intersectionBirdViewForegroundPoints);
    auto x = position[0], y = position[1];
    trace[trace_index % historyTraceSize] = cv::Point(x, y);
    auto theta = std::atan2(y - previousPoint.y, x - previousPoint.x) * 180 / PI;
    previousPoint = cv::Point(x, y);
    trace_index++;

    std::cout << "position: " << x << " " << y << " " << theta << std::endl;

    cv::imshow("intersection", intersectionBirdViewForegroundFrame);
    drawTrace(birdViewFrame[0], trace);
    cv::imshow(windowLabels[0] + "birdview", birdViewFrame[0]);
    drawTrace(birdViewFrame[1], trace);
    cv::imshow(windowLabels[1] + "birdview", birdViewFrame[1]);

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
