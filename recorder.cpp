//
// Created by dailydreamer on 17-7-13.
//

#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    const int cameraNum = 2;
    VideoCapture cap[cameraNum];
    Mat camFrame[cameraNum];
    string windowLabels[cameraNum];
    VideoWriter writer[cameraNum];
    const int codec = CV_FOURCC('M', 'J', 'P', 'G');
    for (int i = 0; i < cameraNum; ++i) {
        cap[i].open(i+1);
        windowLabels[i] = "camera" + std::to_string(i);
        if (!cap[i].isOpened()) {
            cout << "ERROR: Cannot open the camera " << i << endl;
            return -1;
        }
        double width = cap[i].get(CV_CAP_PROP_FRAME_WIDTH);
        double height = cap[i].get(CV_CAP_PROP_FRAME_HEIGHT);
        double fps = cap[i].get(CV_CAP_PROP_FPS);
        writer[i].open(windowLabels[i]+".avi", codec, static_cast<int>(fps), Size(static_cast<int>(width), static_cast<int>(height)), true);
        if (!writer[i].isOpened()) {
            cout << "ERROR: Failed to open the video writer " << i << endl;
            return -1;
        }
        namedWindow(windowLabels[i],CV_WINDOW_AUTOSIZE);
    }

    cout << "Start Record!" << endl;

    for (int key = 0; key != 'q' && key != 27; ) {
        for (int i = 0; i < cameraNum; i++) {
            bool success = cap[i].read(camFrame[i]);
            if (!success) {
                cout << "ERROR: Cannot read a frame from video file " << i << endl;
                continue;
            }
            writer[i].write(camFrame[i]);
            imshow(windowLabels[i], camFrame[i]);
            key = cv::waitKey(100);
        }
    }
    return 0;
}
