#include "2005107_Ray.h"

Ray::Ray() : origin(Vector3D::zero()), direction(Vector3D::one())
{
    direction.normalize();
}

Ray::Ray(Vector3D originParam, Vector3D directionParam) : origin(originParam), direction(directionParam)
{
    direction.normalize();
}

Ray Ray::setOrigin(Vector3D originParam)
{
    origin = originParam;
    return *this;
}

Ray Ray::setDirection(Vector3D directionParam)
{
    direction = directionParam;
    return *this;
}

Vector3D Ray::getOrigin() const
{
    return origin;
}

Vector3D Ray::getDirection() const
{
    return direction;
}

ostream &operator<<(ostream &out, const Ray &r)
{
    out << "Origin: " << r.getOrigin() << " Direction: " << r.getDirection();
    return out;
}