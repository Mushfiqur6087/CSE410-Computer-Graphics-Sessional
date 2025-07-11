#include "2005107_Color.h"
#include <algorithm>

// Default constructor
Color::Color() : red(0.0), green(0.0), blue(0.0)
{
}

// Parameterized constructor
Color::Color(double red, double green, double blue) : red(red), green(green), blue(blue)
{
}

// Copy constructor
Color::Color(const Color& other) : red(other.red), green(other.green), blue(other.blue)
{
}

// Assignment operator
Color& Color::operator=(const Color& other)
{
    if (this != &other)
    {
        red = other.red;
        green = other.green;
        blue = other.blue;
    }
    return *this;
}

// Getters
double Color::getRed() const
{
    return red;
}

double Color::getGreen() const
{
    return green;
}

double Color::getBlue() const
{
    return blue;
}

// Setters
void Color::setRed(double red)
{
    this->red = red;
}

void Color::setGreen(double green)
{
    this->green = green;
}

void Color::setBlue(double blue)
{
    this->blue = blue;
}

void Color::setColor(double red, double green, double blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
}

// Utility methods
Color Color::operator+(const Color& other) const
{
    return Color(red + other.red, green + other.green, blue + other.blue);
}

Color Color::operator-(const Color& other) const
{
    return Color(red - other.red, green - other.green, blue - other.blue);
}

Color Color::operator*(double scalar) const
{
    return Color(red * scalar, green * scalar, blue * scalar);
}

Color Color::operator/(double scalar) const
{
    if (scalar != 0.0)
    {
        return Color(red / scalar, green / scalar, blue / scalar);
    }
    return *this;
}

Color& Color::operator+=(const Color& other)
{
    red += other.red;
    green += other.green;
    blue += other.blue;
    return *this;
}

Color& Color::operator-=(const Color& other)
{
    red -= other.red;
    green -= other.green;
    blue -= other.blue;
    return *this;
}

Color& Color::operator*=(double scalar)
{
    red *= scalar;
    green *= scalar;
    blue *= scalar;
    return *this;
}

Color& Color::operator/=(double scalar)
{
    if (scalar != 0.0)
    {
        red /= scalar;
        green /= scalar;
        blue /= scalar;
    }
    return *this;
}

// Clamp color values to [0, 1] range
void Color::clamp()
{
    red = std::max(0.0, std::min(1.0, red));
    green = std::max(0.0, std::min(1.0, green));
    blue = std::max(0.0, std::min(1.0, blue));
}

// Stream operator
ostream& operator<<(ostream& out, const Color& color)
{
    out << "Color: " << color.red << " " << color.green << " " << color.blue;
    return out;
}
