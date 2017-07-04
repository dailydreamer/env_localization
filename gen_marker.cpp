//
// Created by dailydreamer on 17-7-4.
//

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgcodecs.hpp>

void generateArucoMarker() {
    cv::Mat markerImage;
    auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    int markerId = 0;
    int markerSize = 200; // px
    int markerBorderSize = 1;
    cv::aruco::drawMarker(dictionary, markerId, markerSize, markerImage, markerBorderSize);
    std::string markerImagePath = "gen_aruco.png";
    cv::imwrite(markerImagePath, markerImage);
}

int main() {
    generateArucoMarker();
    return 0;
}