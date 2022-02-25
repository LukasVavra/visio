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

int canny_ltres = 50;
int canny_htres = 200;
int hough_minlength = 30;
int hough_maxgap = 10;

int main() {
  // original image matrix
  Mat image;
  // gray image matrix
  Mat gray;
  // edges matrix
  Mat edges;
  // line vector
  vector<Vec4i> lines;
  // points
  Point p1, p2;
  // capture device
  VideoCapture cap;
  int deviceID = 0;
  int apiID = CAP_ANY;

  // try to open capture device
  cap.open(deviceID, apiID);
  if(!cap.isOpened()) {
    cerr << "Cannot open capture device!";
    return 1;
  }

  cout << "Backend API name: " << cap.getBackendName() << endl;
  cout << "Starting video stream.\n";
  cout << "Press any key to stop streaming...\n";

  while(true) {
    // read image
    cap >> image;
    // convert to grayscale
    cvtColor(image, gray, COLOR_BGR2GRAY);
    // find edges
    Canny(gray, edges, canny_ltres, canny_htres);
    // detect lines
    HoughLinesP(edges, lines, 1, CV_PI/90, 80, hough_minlength, hough_maxgap);
    // draw lines into original image
    for(size_t i=0; i < lines.size(); i++) {
      p1 = Point(lines[i][0], lines[i][1]);
      p2 = Point(lines[i][2], lines[i][3]);
      line(image, p1, p2, Scalar(0,0,255), 3);
    }
    // show edges
    imshow("Project Visio", image);
    // wait for keypress
    int wk = waitKey(5);
    if(wk >= 0) {
      cout << "Key pressed:" << wk << endl;
      if(wk == 27) break;       // ESC
      if(wk == 13) load_cfg();  // Enter
    }
  }
  cout << "Stream terminated.";
  return 0;
}
