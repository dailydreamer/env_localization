//
// Created by dailydreamer on 17-6-8.
//


#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include "ipm.h"

// public

Ipm::Ipm(const cv::Size &inputSize, const cv::Size &outputSize, const std::vector<cv::Point2f>& inputPoints, const std::vector<cv::Point2f>& outputPoints) {
    auto inv_homography = cv::findHomography(outputPoints, inputPoints);
    createMaps(inputSize, outputSize, inv_homography);
}

void Ipm::applyHomography(const cv::Mat inputImage, cv::Mat& outputImage, int borderMode) {
    cv::remap(inputImage, outputImage, _map_x, _map_y, cv::INTER_LINEAR, borderMode);
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

void Ipm::createMaps(const cv::Size &inputSize, const cv::Size &outputSize, const cv::Mat inv_homography) {
    _map_x.create(outputSize, CV_32F);
    _map_y.create(outputSize, CV_32F);
    for( int j = 0; j < outputSize.height; ++j )
    {
        float* ptRowX = _map_x.ptr<float>(j);
        float* ptRowY = _map_y.ptr<float>(j);
        for( int i = 0; i < outputSize.width; ++i )
        {
            cv::Point2f pt = applyHomography( cv::Point2f( static_cast<float>(i), static_cast<float>(j) ), inv_homography );
            ptRowX[i] = pt.x;
            ptRowY[i] = pt.y;
        }
    }
}