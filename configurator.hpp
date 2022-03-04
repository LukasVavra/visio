#ifndef __CONFIGURATOR_HPP__
#define __CONFIGURATOR_HPP__

using namespace std;

// variables
extern int thresh_val;
extern int ksize_x;
extern int ksize_y;
extern int bin_inv;

// functions
string get_var(string line);
int get_val(string line);
void parse(string line);
void load_cfg();

#endif
