#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include "ipm.h"

bool readIntrinsic(std::string filename, cv::Mat& camMatrix, cv::Mat& distCoeffs) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if(!fs.isOpened()) {
        fs.release();
        return false;
    }
    fs["camera_matrix"] >> camMatrix;
    fs["distortion_coefficients"] >> distCoeffs;
    fs.release();
    return true;
}

bool readExtrinsic(std::string filename, std::vector<cv::Vec3d>& rvecs, std::vector<cv::Vec3d>& tvecs) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if(!fs.isOpened()) {
        fs.release();
        return false;
    }
    int size;
    fs["size"] >> size;
    cv::Vec3d tmpRvec, tmpTvec;
    for (int i = 0; i < size; i++) {
        fs["rvec_" + std::to_string(i)] >> tmpRvec;
        fs["tvec_" + std::to_string(i)] >> tmpTvec;
        rvecs.push_back(tmpRvec);
        tvecs.push_back(tmpTvec);
    }
    fs.release();
    return true;
}

void subtractBackground(std::string videoPath, bool debug = false) {
    cv::namedWindow("frame");
    cv::namedWindow("foreground");

   // auto backgroundSubtractorPtr = cv::bgsegm::createBackgroundSubtractorGMG(10, 0.9);
    auto backgroundSubtractorPtr = cv::bgsegm::createBackgroundSubtractorMOG();


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
}

int main() {

//    std::string videoPath = "./Video_001.avi";
//    subtractBackground(videoPath, false);s


    cv::Mat inputImage = cv::imread("aruco.png");
    Ipm ipm("./inv_map.yml");

    cv::Mat birdViewImage;
   // cv::cvtColor(inputImage, inputImage, CV_BGR2GRAY);
    cv::namedWindow("input");
    cv::imshow("input", inputImage);

    ipm.getIpmImage(inputImage, birdViewImage);
    cv::namedWindow("birdview");
    cv::imshow("birdview", birdViewImage);

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}