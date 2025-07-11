#include "2005107_Object.h"
#include <algorithm>
#include <cmath>
#include <vector>
using namespace std;

// Include the specific light headers
#include "../PointLight/2005107_PointLight.h"
#include "../SpotLight/2005107_SpotLight.h"

// Forward declaration for radianToDegree function
double radianToDegree(double radian);

extern double epsilon;
extern double recursionLevel;
extern double zNear, zFar;
extern vector<Object *> objects;
extern vector<PointLight *> pointLights;
extern vector<SpotLight *> spotLights;

extern Vector3D initialCameraPosition;
extern Vector3D initialCameraLook;

Object::Object() : referencePoint(Vector3D::zero()), height(0), width(0), length(0), shine(0)
{
    color = Color(0, 0, 0);
    materialCoefficients.setCoefficients(0, 0, 0, 0);
}

Object Object::setColor(Color color)
{
    this->color = color;
    return *this;
}

Object Object::setShine(int shine)
{
    this->shine = shine;
    return *this;
}

Object Object::setCoefficients(Coefficients coefficients)
{
    materialCoefficients = coefficients;
    return *this;
}

Color Object::getSurfaceColor(Vector3D point)
{
    return color;
}

Coefficients Object::getCoefficients()
{
    return materialCoefficients;
}

Vector3D Object::computeNormal(Vector3D point)
{
    return Vector3D(-1.0, -1.0, -1.0);
}

void Object::draw()
{
    cout << "Object " << *this << endl;
}

double Object::intersect(Ray *ray)
{
    return -1.0;
}

void Object::phongLighting(Ray *ray, Color *color, int level)
{
    double tmin = intersect(ray);
    if (tmin < 0)
    {
        return;
    }
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * tmin;
    Color intersectionPointColor = getSurfaceColor(intersectionPoint);

    color->setRed(intersectionPointColor.getRed() * materialCoefficients.getAmbient());
    color->setGreen(intersectionPointColor.getGreen() * materialCoefficients.getAmbient());
    color->setBlue(intersectionPointColor.getBlue() * materialCoefficients.getAmbient());

    Ray normalRay = Ray(intersectionPoint, computeNormal(intersectionPoint));
    Ray observerRay = *ray;

    // Point lights contribution
    computePointLightContribution(intersectionPoint, &observerRay, color, intersectionPointColor);
    
    // Spot lights contribution
    computeSpotLightContribution(intersectionPoint, &observerRay, color, intersectionPointColor);
    
    // Recursive reflection
    computeReflection(intersectionPoint, &observerRay, color, level);
}

// Helper method implementations
void Object::computePointLightContribution(Vector3D intersectionPoint, Ray *observerRay, Color *color, Color intersectionPointColor)
{
    Ray normalRay = Ray(intersectionPoint, computeNormal(intersectionPoint));
    
    for (PointLight *pointLight : pointLights)
    {
        Ray incidentRay = Ray(pointLight->getLightPosition(), intersectionPoint - pointLight->getLightPosition());
        Vector3D reflectedDirection = getReflectionDirection(incidentRay.getDirection(), normalRay.getDirection());
        Ray reflectedRay = Ray(intersectionPoint, reflectedDirection);

        double distance = (pointLight->getLightPosition() - intersectionPoint).length();
        if (distance < epsilon)
        {
            continue;
        }

        if (!isInShadow(intersectionPoint, pointLight->getLightPosition(), distance))
        {
            double diffuse = computeDiffuseComponent(incidentRay.getDirection(), normalRay.getDirection());
            double specular = computeSpecularComponent(reflectedRay.getDirection(), observerRay->getDirection(), shine);

            // diffuse reflection
            color->setRed(color->getRed() + pointLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getDiffuse() * diffuse));
            color->setGreen(color->getGreen() + pointLight->getColor().getGreen() * intersectionPointColor.getGreen() * (materialCoefficients.getDiffuse() * diffuse));
            color->setBlue(color->getBlue() + pointLight->getColor().getBlue() * intersectionPointColor.getBlue() * (materialCoefficients.getDiffuse() * diffuse));

            // specular reflection
            color->setRed(color->getRed() + pointLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getSpecular() * pow(specular, shine)));
            color->setGreen(color->getGreen() + pointLight->getColor().getGreen() * intersectionPointColor.getGreen() * (materialCoefficients.getSpecular() * pow(specular, shine)));
            color->setBlue(color->getBlue() + pointLight->getColor().getBlue() * intersectionPointColor.getBlue() * (materialCoefficients.getSpecular() * pow(specular, shine)));
        }
    }
}

void Object::computeSpotLightContribution(Vector3D intersectionPoint, Ray *observerRay, Color *color, Color intersectionPointColor)
{
    Ray normalRay = Ray(intersectionPoint, computeNormal(intersectionPoint));
    
    for (SpotLight *spotLight : spotLights)
    {
        Ray incidentRay = Ray(spotLight->getLightPosition(), intersectionPoint - spotLight->getLightPosition());
        Vector3D reflectedDirection = getReflectionDirection(incidentRay.getDirection(), normalRay.getDirection());
        Ray reflectedRay = Ray(intersectionPoint, reflectedDirection);

        double distance = (spotLight->getLightPosition() - intersectionPoint).length();
        if (distance < epsilon)
        {
            continue;
        }

        double beta = acos(incidentRay.getDirection() * spotLight->getLightDirection());
        beta = radianToDegree(beta);
        if (fabs(beta) > spotLight->getCutoffAngle())
        {
            continue;
        }

        if (!isInShadow(intersectionPoint, spotLight->getLightPosition(), distance))
        {
            double diffuse = computeDiffuseComponent(incidentRay.getDirection(), normalRay.getDirection());
            double specular = computeSpecularComponent(reflectedRay.getDirection(), observerRay->getDirection(), shine);

            // diffuse reflection
            color->setRed(color->getRed() + spotLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getDiffuse() * diffuse));
            color->setGreen(color->getGreen() + spotLight->getColor().getGreen() * intersectionPointColor.getGreen() * (materialCoefficients.getDiffuse() * diffuse));
            color->setBlue(color->getBlue() + spotLight->getColor().getBlue() * intersectionPointColor.getBlue() * (materialCoefficients.getDiffuse() * diffuse));

            // specular reflection
            color->setRed(color->getRed() + spotLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getSpecular() * pow(specular, shine)));
            color->setGreen(color->getGreen() + spotLight->getColor().getGreen() * intersectionPointColor.getGreen() * (materialCoefficients.getSpecular() * pow(specular, shine)));
            color->setBlue(color->getBlue() + spotLight->getColor().getBlue() * intersectionPointColor.getBlue() * (materialCoefficients.getSpecular() * pow(specular, shine)));
        }
    }
}

void Object::computeReflection(Vector3D intersectionPoint, Ray *observerRay, Color *color, int level)
{
    if (level >= recursionLevel)
    {
        return;
    }
    
    Ray normalRay = Ray(intersectionPoint, computeNormal(intersectionPoint));
    Vector3D reflectedDirection = getReflectionDirection(observerRay->getDirection(), normalRay.getDirection());
    Ray reflectedViewRay = Ray(intersectionPoint, reflectedDirection);
    reflectedViewRay.setOrigin(reflectedViewRay.getOrigin() + reflectedViewRay.getDirection() * epsilon);

    Color *reflectedColor = new Color(0, 0, 0);
    double tmin2 = -1;
    Object *nearestObject = nullptr;

    for (auto obj : objects)
    {
        double t = obj->intersect(&reflectedViewRay);
        if ((t > 0) && (t < tmin2 || (nearestObject == nullptr)))
        {
            tmin2 = t;
            nearestObject = obj;
        }
    }

    if (nearestObject != nullptr && isPointVisible(reflectedViewRay.getOrigin() + reflectedViewRay.getDirection() * tmin2))
    {
        nearestObject->phongLighting(&reflectedViewRay, reflectedColor, level + 1);
        color->setRed(color->getRed() + reflectedColor->getRed() * materialCoefficients.getReflection());
        color->setGreen(color->getGreen() + reflectedColor->getGreen() * materialCoefficients.getReflection());
        color->setBlue(color->getBlue() + reflectedColor->getBlue() * materialCoefficients.getReflection());
    }

    color->red = std::min(1.0, color->red);
    color->green = std::min(1.0, color->green);
    color->blue = std::min(1.0, color->blue);
    color->red = std::max(0.0, color->red);
    color->green = std::max(0.0, color->green);
    color->blue = std::max(0.0, color->blue);
}

bool Object::isInShadow(Vector3D intersectionPoint, Vector3D lightPosition, double lightDistance)
{
    Ray shadowRay = Ray(lightPosition, intersectionPoint - lightPosition);
    
    for (Object *object : objects)
    {
        double t = object->intersect(&shadowRay);
        if (t > 0 && t + epsilon < lightDistance)
        {
            return true;
        }
    }
    return false;
}

double Object::computeDiffuseComponent(Vector3D incidentDirection, Vector3D normalDirection)
{
    return std::max(-(incidentDirection * normalDirection), 0.0);
}

double Object::computeSpecularComponent(Vector3D reflectedDirection, Vector3D observerDirection, int shininess)
{
    return std::max(-(reflectedDirection * observerDirection), 0.0);
}

bool Object::isPointVisible(Vector3D point)
{
    double tAlongLook = (point - initialCameraPosition) * initialCameraLook;
    return (tAlongLook >= zNear && tAlongLook <= zFar);
}

Vector3D Object::getReflectionDirection(Vector3D incidentDirection, Vector3D normalDirection)
{
    return incidentDirection - normalDirection * (2 * (incidentDirection * normalDirection));
}

ostream &operator<<(ostream &out, const Object &o)
{
    out << "Reference Point: " << o.referencePoint << " Height: " << o.height << " Width: " << o.width << " Length: " << o.length << " Color: " << o.color.getRed() << " " << o.color.getGreen() << " " << o.color.getBlue() << " Coefficients: " << o.materialCoefficients.getAmbient() << " " << o.materialCoefficients.getDiffuse() << " " << o.materialCoefficients.getReflection() << " " << o.materialCoefficients.getSpecular() << " Shine: " << o.shine;
    return out;
}