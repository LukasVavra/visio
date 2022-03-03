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
int canny_ltres = 50;
int canny_htres = 200;
int hough_minlength = 30;
int hough_maxgap = 10;
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
  // contours
  Mat contours;
  // Canny edges matrix
  Mat canny;
  // line vector
  vector<Vec4i> lines;
  // contour vector
  vector<vector<Point>> contoursThresh, contoursCanny;
  // minimum area bounding box
  Point2f mbb[4];
  // points
  Point p1, p2;
  // capture device
  VideoCapture cap;
  int deviceID = 0;
  int apiID = CAP_ANY;

  if(argc == 2) {
    deviceID = atoi(argv[1]);
  }

  // try to open capture device
  cap.open(deviceID, apiID);
  if(!cap.isOpened()) {
    cerr << "Cannot open capture device!";
    return 1;
  }

  cout << "Loading config file" << endl;
  load_cfg();

  cout << "Backend API name: " << cap.getBackendName() << endl;
  cout << "Starting video stream.\n";
  cout << "Press <ESC> or <q> to stop streaming...\n";

  // create window
  namedWindow(winName, WINDOW_AUTOSIZE);
  // create trackbars
  createTrackbar("Threshold", winName, &thresh_val, 255, nullptr);

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
    // find edges
    //Canny(gray, canny, canny_ltres, canny_htres);
    // detect contours
    findContours(thresh, contoursThresh, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    //findContours(canny, contoursCanny, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // draw contours
    for(size_t i = 0; i < contoursThresh.size(); i++) {
      drawContours(image, contoursThresh, i, Scalar(255, 0, 0));
    }
    /*
    for(size_t i = 0; i < contoursCanny.size(); i++) {
      drawContours(image, contoursCanny, i, Scalar(0,0,255));
    }
    */
    for(size_t i = 0; i < contoursThresh.size(); i++) {
      // find the minimum area enclosing box
      RotatedRect box = minAreaRect(contoursThresh[i]);
      box.points(mbb);
      // draw box
      for(int j = 0; j < 4; j++) {
        line(image, mbb[j], mbb[(j+1)%4], Scalar(0, 0, 255),  2);
      }
      // write an angle
      string angle = format("%.1f", box.angle);
      putText(image, angle, box.center,  FONT_HERSHEY_SIMPLEX,  0.4,  Scalar(0, 255, 0));
    }
    /*
    // detect lines
    HoughLinesP(edges, lines, 1, CV_PI/180, 80, hough_minlength, hough_maxgap);
    // draw lines into original image
    for(size_t i=0; i < lines.size(); i++) {
      p1 = Point(lines[i][0], lines[i][1]);
      p2 = Point(lines[i][2], lines[i][3]);
      line(image, p1, p2, Scalar(0,0,255), 3);
    }*/
    // show edges
    imshow(winName, orig);
    imshow("Robot vision", thresh);
    //imshow("Canny image", canny);
    // wait for keypress
    int wk = waitKey(1);
    if(wk >= 0) {
      cout << "Key pressed:" << wk << endl;
      if(wk == 27 || wk == 113) break;       // ESC, q
      if(wk == 13) load_cfg();  // Enter
    }
  }
  cout << "Stream terminated.";
  return 0;
}
