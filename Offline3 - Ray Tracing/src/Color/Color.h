#ifndef COLOR_H
#define COLOR_H

#include <iostream>
using namespace std;

class Color {
public:
    double color[3];
    double r, g, b;
    
    Color() : r(0), g(0), b(0) {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }
    
    Color(double red, double green, double blue) : r(red), g(green), b(blue) {
        color[0] = red;
        color[1] = green;
        color[2] = blue;
    }
    
    // Friend function for output stream operator
    friend ostream &operator<<(ostream &out, const Color &color);
};

#endif
