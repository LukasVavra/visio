#ifndef __CONFIGURATOR_HPP__
#define __CONFIGURATOR_HPP__

using namespace std;

// variables
extern int canny_ltres;
extern int canny_htres;
extern int hough_minlength;
extern int hough_maxgap;


// functions
string get_var(string line);
int get_val(string line);
void parse(string line);
void load_cfg();

#endif