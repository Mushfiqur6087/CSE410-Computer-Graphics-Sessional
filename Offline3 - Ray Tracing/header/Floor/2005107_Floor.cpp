#include "2005107_Floor.h"

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

Floor::Floor(double tileCount, double tileSize, double height) : Object(), tileCount(tileCount), tileSize(tileSize)
{
    referencePoint = Vector3D(-tileCount * tileSize / 2, -tileCount * tileSize / 2, 0);
    this->height = height;
    width = tileCount * tileSize;
    length = tileCount * tileSize;
    tileColor1 = Color(1, 1, 1);
    tileColor2 = Color(0, 0, 0);
    textureImage = nullptr;
    useTexture = false;
}

Floor::~Floor()
{
    if (textureImage != nullptr) {
        delete textureImage;
        textureImage = nullptr;
    }
}

void Floor::draw()
{
    int startX = -tileCount * tileSize / 2;
    int startY = -tileCount * tileSize / 2;
    glBegin(GL_QUADS);
    {
        for (int i = 0; i < tileCount; i++)
        {
            for (int j = 0; j < tileCount; j++)
            {
                if ((i + j) % 2 == 0)
                {
                    glColor3f(tileColor1.getRed(), tileColor1.getGreen(), tileColor1.getBlue());
                }
                else
                {
                    glColor3f(tileColor2.getRed(), tileColor2.getGreen(), tileColor2.getBlue());
                }

                glVertex3f(startX + i * tileSize, startY + j * tileSize, height);
                glVertex3f(startX + (i + 1) * tileSize, startY + j * tileSize, height);
                glVertex3f(startX + (i + 1) * tileSize, startY + (j + 1) * tileSize, height);   
                glVertex3f(startX + i * tileSize, startY + (j + 1) * tileSize, height);
            }
        }
    }
    glEnd();
}

Vector3D Floor::computeNormal(Vector3D point)
{
    return Vector3D::forward();
}

double Floor::intersect(Ray *ray)
{
    double t = (height - ray->getOrigin().z) / ray->getDirection().z;
    if (t < 0)
    {
        return -1.0;
    }
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * t;
    if (intersectionPoint.x < referencePoint.x || intersectionPoint.x > referencePoint.x + width || intersectionPoint.y < referencePoint.y || intersectionPoint.y > referencePoint.y + length)
    {
        return -1.0;
    }
    return t;
}

Color Floor::getSurfaceColor(Vector3D point)
{
    if (useTexture && textureImage != nullptr) {
        // Calculate which tile we're in
        int tileX = (int)((point.x - referencePoint.x) / tileSize);
        int tileY = (int)((point.y - referencePoint.y) / tileSize);
        
        // Calculate position within the tile (0 to 1)
        double localX = (point.x - referencePoint.x) - tileX * tileSize;
        double localY = (point.y - referencePoint.y) - tileY * tileSize;
        
        // Convert to texture coordinates (0 to 1) within the tile
        double u = localX / tileSize;
        double v = localY / tileSize;
        
        // Clamp u and v to [0, 1] range
        u = max(0.0, min(1.0, u));
        v = max(0.0, min(1.0, v));
        
        return sampleTexture(u, v);
    } else {
        // Default checkerboard pattern
        int x = (point.x - referencePoint.x) / tileSize;
        int y = (point.y - referencePoint.y) / tileSize;
        if ((x + y) % 2 == 0) {
            return tileColor1;
        } else {
            return tileColor2;
        }
    }
}

void Floor::setTexture(const string& texturePath)
{
    // Clean up existing texture if any
    if (textureImage != nullptr) {
        delete textureImage;
        textureImage = nullptr;
    }
    
    // Load new texture
    textureImage = new bitmap_image(texturePath);
    
    // Check if texture loaded successfully
    if (textureImage->width() > 0 && textureImage->height() > 0) {
        useTexture = true;
        cout << "Texture loaded successfully: " << texturePath << " (" << textureImage->width() << "x" << textureImage->height() << ")" << endl;
    } else {
        cout << "Failed to load texture: " << texturePath << endl;
        delete textureImage;
        textureImage = nullptr;
        useTexture = false;
    }
}

void Floor::disableTexture()
{
    useTexture = false;
    if (textureImage != nullptr) {
        delete textureImage;
        textureImage = nullptr;
    }
}

Color Floor::sampleTexture(double u, double v)
{
    if (!textureImage || textureImage->width() <= 0 || textureImage->height() <= 0) {
        return Color(0.5, 0.5, 0.5); // Gray fallback
    }
    
    // Clamp u and v to [0,1]
    u = max(0.0, min(1.0, u));
    v = max(0.0, min(1.0, v));
    
    // Normalized -> pixel coords
    int pixelX = (int)(u * (textureImage->width() - 1));
    int pixelY = (int)((1.0 - v) * (textureImage->height() - 1)); // Flip Y
    
    // Safety clamp
    pixelX = max(0, min((int)textureImage->width() - 1, pixelX));
    pixelY = max(0, min((int)textureImage->height() - 1, pixelY));
    
    // Get pixel color
    unsigned char r, g, b;
    textureImage->get_pixel(pixelX, pixelY, r, g, b);
    
    Color color;
    color.red = r / 255.0;
    color.green = g / 255.0;
    color.blue = b / 255.0;
    
    return color;
}
