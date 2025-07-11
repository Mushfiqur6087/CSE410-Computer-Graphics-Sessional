#pragma once

#include <iostream>
using namespace std;

class Color
{
public:
    double red, green, blue;

    // Default constructor
    Color();
    
    // Parameterized constructor
    Color(double red, double green, double blue);
    
    // Copy constructor
    Color(const Color& other);
    
    // Assignment operator
    Color& operator=(const Color& other);
    
    // Getters
    double getRed() const;
    double getGreen() const;
    double getBlue() const;
    
    // Setters
    void setRed(double red);
    void setGreen(double green);
    void setBlue(double blue);
    void setColor(double red, double green, double blue);
    
    // Utility methods
    Color operator+(const Color& other) const;
    Color operator-(const Color& other) const;
    Color operator*(double scalar) const;
    Color operator/(double scalar) const;
    Color& operator+=(const Color& other);
    Color& operator-=(const Color& other);
    Color& operator*=(double scalar);
    Color& operator/=(double scalar);
    
    // Clamp color values to [0, 1] range
    void clamp();
    
    // Stream operator
    friend ostream& operator<<(ostream& out, const Color& color);
};
