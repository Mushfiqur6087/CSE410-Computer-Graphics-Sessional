#include "2005107_SpotLight.h"

SpotLight::SpotLight() : lightDirection(1, 1, 1), cutoffAngle(0)
{
    lightDirection.normalize();
}

SpotLight SpotLight::setLightPosition(double x, double y, double z)
{
    pointLight.setLightPosition(x, y, z);
    return *this;
}

SpotLight SpotLight::setColor(Color color)
{
    pointLight.setColor(color);
    return *this;
}

SpotLight SpotLight::setLightDirection(double x, double y, double z)
{
    lightDirection.x = x;
    lightDirection.y = y;
    lightDirection.z = z;
    lightDirection.normalize();
    return *this;
}

SpotLight SpotLight::setCutoffAngle(double angle)
{
    cutoffAngle = angle;
    return *this;
}

Vector3D SpotLight::getLightPosition() const
{
    return pointLight.getLightPosition();
}

Color SpotLight::getColor() const
{
    return pointLight.getColor();
}

Vector3D SpotLight::getLightDirection() const
{
    return lightDirection;
}

double SpotLight::getCutoffAngle() const
{
    return cutoffAngle;
}

void SpotLight::draw()
{
    pointLight.draw();
}

ostream &operator<<(ostream &out, const SpotLight &s)
{
    cout << s.pointLight << " Direction: " << s.lightDirection << " Cutoff Angle: " << s.cutoffAngle;
    return out;
}
