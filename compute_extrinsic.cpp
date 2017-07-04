//
// Created by dailydreamer on 17-7-4.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/aruco.hpp>
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

bool writeExtrinsic(std::string filename, std::vector<cv::Vec3d> rvecs, std::vector<cv::Vec3d> tvecs) {
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if(!fs.isOpened()) {
        fs.release();
        return false;
    }
    assert(rvecs.size() == tvecs.size());
    fs << "size" << static_cast<int>(rvecs.size());
    for (int i = 0; i < rvecs.size(); ++i) {
        fs << "rvec_" + std::to_string(i) << rvecs[i];
        fs << "tvec_" + std::to_string(i) << tvecs[i];
    }
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

void computeExtrinsic(std::string calibImagePath, float markerLength, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, bool debug = false) {
    cv::Mat calibImage = cv::imread(calibImagePath);
    //cv::resize(calibImage, calibImage, cv::Size(800, 600));

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> corners, rejected;
    auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    auto detectorParams = cv::aruco::DetectorParameters::create();
    detectorParams->cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
    cv::aruco::detectMarkers(calibImage, dictionary, corners, markerIds, detectorParams, rejected);

    std::cout <<"marker: " << markerIds.size() << " rejected: " << rejected.size() << std::endl;

    std::vector<cv::Vec3d> rvecs, tvecs;
    cv::aruco::estimatePoseSingleMarkers(corners, markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);
    bool writeSuccess = writeExtrinsic("./extrinsic.yml", rvecs, tvecs);
    if (!writeSuccess) {
        std::cout << "write extrinsic failed" << std::endl;
    }

    if (debug) {
        cv::Mat resImage;
        calibImage.copyTo(resImage);

        cv::aruco::drawDetectedMarkers(resImage, corners, markerIds);
        cv::aruco::drawDetectedMarkers(resImage, rejected);

        std::cout << cameraMatrix << std::endl << distCoeffs << std::endl;

        std::cout << rvecs[0] << std::endl << tvecs[0] << std::endl;
        for(auto i = 0; i < markerIds.size(); ++i) {
            std::cout << i << std::endl;
            cv::aruco::drawAxis(resImage, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], markerLength * 0.5f);
        }

        cv::namedWindow("res", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::imshow("res", resImage);
    }
}

int main() {

    // read intrinsic
    cv::Mat cameraMatrix, distCoeffs;
    bool readIntrinsicSuccess = readIntrinsic("./intrinsic.yml", cameraMatrix, distCoeffs);
    if (!readIntrinsicSuccess) {
        std::cout << "read intrinsic failed" << std::endl;
    }

    // compute extrinsic
    float markerLength = 140;    // 140 mm, 1mm = 1px
    computeExtrinsic("aruco.png", markerLength, cameraMatrix, distCoeffs);

    // read extrinsic
    std::vector<cv::Vec3d> rvecs, tvecs;
    bool readExtrinsicSuccess = readExtrinsic("./extrinsic.yml", rvecs, tvecs);
    if (!readExtrinsicSuccess) {
        std::cout << "read extrinsicc failed" << std::endl;
    }
    std::cout << "rvecs: " << rvecs[0] << " tvecs: " << tvecs[0] << std::endl;

    // compute Ipm
    cv::Mat inputImage = cv::imread("aruco.png");
    Ipm ipm(inputImage.size(), rvecs[0], tvecs[0], cameraMatrix, distCoeffs);
    bool writeSuccess = ipm.writeMaps("./inv_map.yml");
    if (!writeSuccess) {
        std::cout << "write inv map failed" << std::endl;
    }
    return 0;
}
