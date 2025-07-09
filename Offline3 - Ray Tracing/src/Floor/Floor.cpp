#include "Floor.h"
#include <iostream>
#include <cmath>
#include <GL/glut.h>

using namespace std;

// Constructor
Floor::Floor(double floorWidth, double tileWidth) : floorWidth(floorWidth), tileWidth(tileWidth) {
    // Set reference point to center the floor at origin
    referencePoint = Vector3(-floorWidth / 2, -floorWidth / 2, 0);
    length = tileWidth;
    width = tileWidth;
    height = 0; // Floor has no height
    
    // Initialize tile colors
    tile_color1 = Color(1.0, 1.0, 1.0); // White tiles
    tile_color2 = Color(0.0, 0.0, 0.0); // Black tiles
}

// Default constructor
Floor::Floor() : floorWidth(1000), tileWidth(20) {
    referencePoint = Vector3(-floorWidth / 2, -floorWidth / 2, 0);
    length = tileWidth;
    width = tileWidth;
    height = 0;
    
    // Initialize tile colors
    tile_color1 = Color(1.0, 1.0, 1.0); // White tiles
    tile_color2 = Color(0.0, 0.0, 0.0); // Black tiles
}

// Override the draw method
void Floor::draw() {
    int tile_count = static_cast<int>(floorWidth / tileWidth);
    int tile_size = tileWidth;
    int start_x = -tile_count * tile_size / 2;
    int start_y = -tile_count * tile_size / 2;
    
    glBegin(GL_QUADS);
    {
        for (int i = 0; i < tile_count; i++)
        {
            for (int j = 0; j < tile_count; j++)
            {
                if ((i + j) % 2 == 0)
                {
                    glColor3f(tile_color1.r, tile_color1.g, tile_color1.b);
                }
                else
                {
                    glColor3f(tile_color2.r, tile_color2.g, tile_color2.b);
                }

                glVertex3f(start_x + i * tile_size, start_y + j * tile_size, height);
                glVertex3f(start_x + (i + 1) * tile_size, start_y + j * tile_size, height);
                glVertex3f(start_x + (i + 1) * tile_size, start_y + (j + 1) * tile_size, height);   
                glVertex3f(start_x + i * tile_size, start_y + (j + 1) * tile_size, height);
            }
        }
    }
    glEnd();
}

// Getter methods
double Floor::getFloorWidth() const {
    return floorWidth;
}

double Floor::getTileWidth() const {
    return tileWidth;
}

// Setter methods
void Floor::setFloorWidth(double width) {
    floorWidth = width;
    referencePoint = Vector3(-floorWidth / 2, -floorWidth / 2, 0);
}

void Floor::setTileWidth(double width) {
    tileWidth = width;
    length = tileWidth;
    this->width = tileWidth;
}

// Utility method to determine if a tile should be white (true) or black (false)
bool Floor::isWhiteTile(double x, double y) const {
    // Convert world coordinates to tile indices
    int tileX = static_cast<int>(floor((x - referencePoint.x) / tileWidth));
    int tileY = static_cast<int>(floor((y - referencePoint.y) / tileWidth));
    
    // Checkerboard pattern: white if sum of indices is even, black if odd
    return (tileX + tileY) % 2 == 0;
}

// Get the color of a tile at given coordinates
Color Floor::getTileColor(double x, double y) const {
    if (isWhiteTile(x, y)) {
        return Color(1.0, 1.0, 1.0); // White tile
    } else {
        return Color(0.0, 0.0, 0.0); // Black tile
    }
}
