#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "configurator.hpp"

using namespace cv;
using namespace std;

// window name
const string winName = "Project Visio";

// random number
RNG rng(255);

// init config values
int thresh_val = 127;
int ksize_x = 11;
int ksize_y = 11;
int bin_inv = 0;

// Generate random color
Scalar randomColor(RNG &rng) {
  int r = (unsigned) rng;
  int g = (unsigned) rng;
  int b = (unsigned) rng;
  return Scalar(r & 255, g & 255, b & 255);
}

// Threshold trackbar callback function
static void callbackThreshold(int value, void* userdata) {
  thresh_val = value;
}
// Inversion trackbar callback function
static void callbackInversion(int value, void* userdata) {
  bin_inv = value;
}

// Main function
int main(int argc, char **argv) {
  // original image matrix
  Mat orig;
  // Resized image
  Mat image;
  // gray image matrix
  Mat gray;
  // smoothed image
  Mat  smooth;
  // thresholded image
  Mat thresh;
  // contour vector
  vector<vector<Point>> contours;
  // maximal contour area
  double maxArea;
  // max area index
  int maxAreaIdx;
  // capture device
  VideoCapture cap;
  int deviceID = 0;
  int apiID = CAP_ANY;

  // device id selected by user
  if(argc == 2) {
    deviceID = atoi(argv[1]);
  }

  // try to open capture device
  cap.open(deviceID, apiID);
  if(!cap.isOpened()) {
    cerr << "Cannot open capture device!";
    return 1;
  }

  // write info to CLI
  cout << "Loading config file" << endl;
  load_cfg();
  cout << "Backend API name: " << cap.getBackendName() << endl;
  cout << "Starting video stream.\n";
  cout << "Press <ESC> or <q> to stop streaming...\n";

  // create window
  namedWindow(winName, WINDOW_AUTOSIZE);
  // create trackbars
  createTrackbar("Threshold", winName, NULL, 255, callbackThreshold);
  // create trackbars
  createTrackbar("Inversion", winName, NULL, 1, callbackInversion);
  // init trackbars value
  setTrackbarPos("Threshold", winName, thresh_val);
  setTrackbarPos("Inversion", winName, bin_inv);

  //  Main  loop
  while(true) {
    // read image
    cap >> orig;
    // get clip from image
    image = orig(Range(orig.rows - 100,orig.rows), Range(0,orig.cols));
    // smooth  image
    //blur(image,  smooth, Size(ksize_x,ksize_y));
    medianBlur(image, smooth, ksize_x);
    // convert to grayscale
    cvtColor(smooth, gray, COLOR_BGR2GRAY);
    // create (inverted) binary image
    if (bin_inv == 1) {
      threshold(gray, thresh, thresh_val, 255, THRESH_BINARY_INV);
    }
    else {
      threshold(gray, thresh, thresh_val, 255, THRESH_BINARY);
    }
    // detect contours
    findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    if(contours.size() > 0) {
      maxArea = 0;
      // find maximal contour area
      for(size_t i = 0; i < contours.size(); i++) {
        double cArea = contourArea(contours[i]);
        if(cArea > maxArea) {
          maxArea = cArea;
          maxAreaIdx = i;
        }
      }
      // draw only maximal contour
      drawContours(image, contours, maxAreaIdx, Scalar(255, 0, 0));
      // draw rotated rectangle over maximal contour
      Point2f mbb[4];
      // find the minimum area enclosing box
      RotatedRect box = minAreaRect(contours[maxAreaIdx]);
      box.points(mbb);
      // draw box
      for(int j = 0; j < 4; j++) {
        line(image, mbb[j], mbb[(j+1)%4], Scalar(0, 0, 255),  2);
      }
      // write an angle
      string angle = format("%.1f deg", box.angle);
      putText(image, angle, box.center,  FONT_HERSHEY_SIMPLEX,  0.4,  Scalar(0, 255, 0));
    }
    // show edges
    imshow(winName, orig);
    imshow("Robot vision", thresh);

    // wait for keypress
    int wk = waitKey(1);
    if(wk >= 0) {
      cout << "Key pressed:" << wk << endl;
      if(wk == 27 || wk == 113) break;       // ESC, q
      if(wk == 13) load_cfg();  // Enter
    }
  }
  // end of main loop
  destroyAllWindows();
  cout << "Stream terminated.";
  return 0;
}
