//
// Created by dailydreamer on 17-6-8.
//

#ifndef ENV_LOCALIZATION_IPM_H
#define ENV_LOCALIZATION_IPM_H

#include <opencv2/core.hpp>

class Ipm {
public:
    Ipm(const cv::Size& inputSize, const cv::Size& outputSize, const std::vector<cv::Point2f>& inputPoints, const std::vector<cv::Point2f>& outputPoints);
    void applyHomography(const cv::Mat inputImage, cv::Mat& outputImage, int borderMode = cv::BORDER_CONSTANT);
    cv::Point2d applyHomography( const cv::Point2d& _point, const cv::Mat& _H );
    cv::Point3d applyHomography( const cv::Point3d& _point, const cv::Mat& _H );
private:
    cv::Mat _map_x, _map_y;
    void createMaps(const cv::Size &inputSize, const cv::Size &outputSize, const cv::Mat inv_homography);
};


#endif //ENV_LOCALIZATION_IPM_H
