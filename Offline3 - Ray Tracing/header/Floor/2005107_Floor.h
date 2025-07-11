#pragma once

#include <iostream>
using namespace std;

#include "../Object/2005107_Object.h"
#include "../Color/2005107_Color.h"
#include "../../bitmap_image.hpp"

class Floor : public Object
{
    double tileCount, tileSize;
    bitmap_image* textureImage;
    bool useTexture;

public:
    Color tileColor1, tileColor2;
    Floor(double tileCount, double tileSize, double height = 0);
    ~Floor();
    void draw();
    Vector3D computeNormal(Vector3D point);
    double intersect(Ray *ray);
    Color getSurfaceColor(Vector3D point);
    void setTexture(const string& texturePath);
    void disableTexture();
    
private:
    Color sampleTexture(double u, double v);
};
