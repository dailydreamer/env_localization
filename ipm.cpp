//
// Created by dailydreamer on 17-6-8.
//

#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include "ipm.h"

// public

Ipm::Ipm(const cv::Size &outputSize, const std::vector<cv::Point2d>& inputPoints, const std::vector<cv::Point2d>& outputPoints) {
    auto inv_homography = cv::findHomography(outputPoints, inputPoints);
    _homography = inv_homography.inv();
    std::cout << "inv_homography" << std::endl << inv_homography << std::endl;
    createMaps(outputSize, inv_homography);
}

Ipm::Ipm(const cv::Size &outputSize, const cv::Vec3d& rvec, const cv::Vec3d tvec, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs) {
    // project world points
    std::vector<cv::Point3d> worldPoints3D;
    worldPoints3D.push_back(cv::Point3d(0, 0, 0));
    worldPoints3D.push_back(cv::Point3d(1, 0, 0));
    worldPoints3D.push_back(cv::Point3d(0, 1, 0));
    worldPoints3D.push_back(cv::Point3d(1, 1, 0));
    std::vector< cv::Point2d > imagePoints;
    cv::projectPoints(worldPoints3D, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    std::vector<cv::Point2d> worldPoints;
    worldPoints.push_back(cv::Point2d(0, 0));
    worldPoints.push_back(cv::Point2d(1, 0));
    worldPoints.push_back(cv::Point2d(0, 1));
    worldPoints.push_back(cv::Point2d(1, 1));

    std::cout << "image points: " << std::endl << imagePoints << std::endl;
    auto inv_homography = cv::findHomography(worldPoints, imagePoints);
    _homography = inv_homography.inv();
    std::cout << "inv_homography" << std::endl << inv_homography << std::endl;
//    inv_homography.at<double>(0,2) -= 100;
//    std::cout << inv_homography.at<double>(0,2) << std::endl;
    createMaps(outputSize, inv_homography);


//    _inv_map.create(outputSize);
//    std::vector<cv::Point2d> pts;
//
//    for( int y = 0; y < outputSize.height; ++y )
//    {
//        auto* ptRow = _inv_map.ptr<cv::Point2f>(y);
//        for( int x = 0; x < outputSize.width; ++x )
//        {
//            cv::projectPoints(std::vector<cv::Point3f> { cv::Point3f( static_cast<float>(x), static_cast<float>(y), 0 ) }, rvec, tvec, cameraMatrix, distCoeffs, pts);
//            ptRow[x] = pts[0];
//        }
//    }
}

void Ipm::getIpmImage(const cv::Mat& inputImage, cv::Mat& outputImage, int borderMode) {
    // use inverse mapping
    cv::remap(inputImage, outputImage, _inv_map, cv::noArray(), cv::INTER_LINEAR, borderMode);
}

void Ipm::getIpmPoints(const std::vector<cv::Point2d>& inputPoints, std::vector<cv::Point2d> outputPoints) {
    for (auto point : inputPoints) {
        outputPoints.push_back(applyHomography(point, _homography));
    }
}

cv::Point2d Ipm::applyHomography( const cv::Point2d& _point, const cv::Mat& _H )
{
    cv::Point2d ret = cv::Point2d( -1, -1 );

    const double u = _H.at<double>(0,0) * _point.x + _H.at<double>(0,1) * _point.y + _H.at<double>(0,2);
    const double v = _H.at<double>(1,0) * _point.x + _H.at<double>(1,1) * _point.y + _H.at<double>(1,2);
    const double s = _H.at<double>(2,0) * _point.x + _H.at<double>(2,1) * _point.y + _H.at<double>(2,2);
    if ( s != 0 )
    {
        ret.x = ( u / s );
        ret.y = ( v / s );
    }
    return ret;
}

cv::Point3d Ipm::applyHomography( const cv::Point3d& _point, const cv::Mat& _H )
{
    cv::Point3d ret = cv::Point3d( -1, -1, 1 );

    const double u = _H.at<double>(0,0) * _point.x + _H.at<double>(0,1) * _point.y + _H.at<double>(0,2) * _point.z;
    const double v = _H.at<double>(1,0) * _point.x + _H.at<double>(1,1) * _point.y + _H.at<double>(1,2) * _point.z;
    const double s = _H.at<double>(2,0) * _point.x + _H.at<double>(2,1) * _point.y + _H.at<double>(2,2) * _point.z;
    if ( s != 0 )
    {
        ret.x = ( u / s );
        ret.y = ( v / s );
    }
    else
        ret.z = 0;
    return ret;
}

// private

void Ipm::createMaps(const cv::Size &outputSize, const cv::Mat inv_homography) {
    _inv_map.create(outputSize);
    for( int y = 0; y < outputSize.height; ++y )
    {
        auto* ptRow = _inv_map.ptr<cv::Point2f>(y);
        for( int x = 0; x < outputSize.width; ++x )
        {
            cv::Point2d pt = applyHomography( cv::Point2d( static_cast<float>(x), static_cast<float>(y) ), inv_homography );
            ptRow[x] = pt;
        }
    }
}