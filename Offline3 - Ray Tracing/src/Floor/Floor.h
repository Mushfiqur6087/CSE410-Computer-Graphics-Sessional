#ifndef FLOOR_H
#define FLOOR_H

#include "../Object/Object.h"

class Floor : public Object {
public:
    double floorWidth;
    double tileWidth;
    Color tile_color1; // White tiles
    Color tile_color2; // Black tiles
    
    // Constructor
    Floor(double floorWidth, double tileWidth);
    
    // Default constructor
    Floor();
    
    // Override the draw method
    void draw() override;
    
    // Getter methods
    double getFloorWidth() const;
    double getTileWidth() const;
    
    // Setter methods
    void setFloorWidth(double width);
    void setTileWidth(double width);
    
    // Utility methods
    bool isWhiteTile(double x, double y) const;
    Color getTileColor(double x, double y) const;
};

#endif // FLOOR_H
