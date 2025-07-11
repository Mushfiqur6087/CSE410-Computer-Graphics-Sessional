#include "2005107_PointLight.h"

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

PointLight::PointLight() : lightPosition(Vector3D::zero())
{
    color = Color(0, 0, 0);
}

PointLight PointLight::setLightPosition(double x, double y, double z)
{
    lightPosition.x = x;
    lightPosition.y = y;
    lightPosition.z = z;
    return *this;
}

PointLight PointLight::setColor(Color color)
{
    this->color = color;
    return *this;
}

Vector3D PointLight::getLightPosition() const
{
    return lightPosition;
}

Color PointLight::getColor() const
{
    return color;
}

void PointLight::draw()
{
    glPushMatrix();
    {
        glColor3f(color.getRed(), color.getGreen(), color.getBlue());
        glTranslatef(lightPosition.x, lightPosition.y, lightPosition.z);
        glutSolidSphere(5, 200, 200);
    }
    glPopMatrix();
}

ostream &operator<<(ostream &out, const PointLight &p)
{
    out << "Light Position: " << p.lightPosition << " Color: " << p.color.getRed() << " " << p.color.getGreen() << " " << p.color.getBlue();
    return out;
}
