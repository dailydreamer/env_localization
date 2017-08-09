//
// Created by dailydreamer on 17-7-4.
//

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgcodecs.hpp>

void generateArucoMarker() {
  cv::Mat markerImage;
  auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
  int markerSize = 600; // px
  int markerBorderSize = 1;
  for (int markerId = 0; markerId < 4; markerId++) {
    cv::aruco::drawMarker(dictionary, markerId, markerSize, markerImage, markerBorderSize);
    std::string markerImagePath = "aruco" + std::to_string(markerId) + ".png";
    cv::imwrite(markerImagePath, markerImage);
  }
}

int main() {
  generateArucoMarker();
  return 0;
}