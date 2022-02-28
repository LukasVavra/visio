#include <iostream>
#include <fstream>
#include <string>
#include "configurator.hpp"

string get_var(string line) {
  size_t pos = line.find("=");
  string v = line.substr(0, pos);
  return v;
}

int get_val(string line) {
  size_t pos = line.find("=");
  string s = line.substr(pos + 1);
  int v = atoi(s.c_str());
  return v;
}

void parse(string line) {
  string var = get_var(line);
  int value = get_val(line);
  cout << "variable:" << var << ",value:" << value << endl;
  if (var.compare("canny_ltres") == 0) canny_ltres = value;
  else if (var.compare("canny_htres") == 0) canny_htres = value;
  else if (var.compare("hough_minlength") == 0) hough_minlength = value;
  else if (var.compare("hough_maxgap") == 0) hough_maxgap = value;
  else if (var.compare("thresh_val") == 0) thresh_val = value;
  else if (var.compare("ksize_x") == 0) ksize_x = value;
  else if (var.compare("ksize_y") == 0) ksize_y = value;
}

void load_cfg() {
  string line;
  ifstream cfgfile("config");

  while(getline(cfgfile, line)) {
    if(line[0] == '#') continue;
    if(line.size() == 0) continue;
    parse(line);
  }

  cfgfile.close();
}
