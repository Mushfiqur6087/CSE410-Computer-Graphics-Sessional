#include <bits/stdc++.h>
#include "helper.h"
#include "bmp_image_codes/bitmap_image.hpp"
using namespace std;

// Fast random number generator for colors (same as Triangle class)
static unsigned long long int g_seed = 1;
inline int fastrand() {
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

struct Color {
    int R, G, B;
    
    Color() {
        R = fastrand() % 256;
        G = fastrand() % 256;
        B = fastrand() % 256;
    }
    
    Color(int r, int g, int b) : R(r), G(g), B(b) {}
    
    void print() const {
        cout << "RGB(" << R << ", " << G << ", " << B << ")";
    }
};

struct MyTriangle {
    Point4 points[3];
    Color triangleColor;  // Single color for entire triangle
    
    MyTriangle() {
        // Initialize with a single random color for the entire triangle
        triangleColor = Color();
    }
    
    void print(int triangleNum) const {
        cout << "Triangle " << triangleNum << ":" << endl;
        for (int i = 0; i < 3; i++) {
            cout << "  Point " << i+1 << ": (" << points[i].x << ", " << points[i].y << ", " << points[i].z << ") ";
        }
        cout << "Color: ";
        triangleColor.print();
        cout << endl;
    }
};

class ZBufferProcessor {
private:
    // Screen dimensions from config.txt
    int Screen_Width, Screen_Height;
    
    // Viewing volume bounds from config.txt
    double leftX, rightX;    // X bounds
    double bottomY, topY;    // Y bounds
    double frontZ, rearZ;    // Z bounds
    
    // Pixel mapping values
    double dx, dy;           // Pixel step sizes
    double Top_Y, Left_X;    // Adjusted top-left corner values
    
    // Buffers
    vector<vector<double>> zBuffer;     // Z-buffer for depth testing
    bitmap_image frameBuffer;           // Frame buffer (bitmap image)
    
    // Data structures
    vector<MyTriangle> triangles;
    
public:
    ZBufferProcessor(const string& configFilename, const string& stage3Filename) {
        readConfiguration(configFilename);
        initializePixelMapping();
        initializeBuffers();
        readTriangles(stage3Filename);
    }
    
    void readConfiguration(const string& configFilename) {
        cout << "Reading configuration from: " << configFilename << endl;
        
        ifstream configFile(configFilename);
        if (!configFile) {
            cerr << "Error: Could not open " << configFilename << endl;
            exit(1);
        }
        
        // Read screen dimensions
        configFile >> Screen_Width >> Screen_Height;
        cout << "Screen dimensions: " << Screen_Width << " x " << Screen_Height << endl;
        
        // Read X limits
        configFile >> leftX;
        rightX = -leftX;  // Right limit is negation of left limit
        cout << "X bounds: [" << leftX << ", " << rightX << "]" << endl;
        
        // Read Y limits
        configFile >> bottomY;
        topY = -bottomY;  // Top limit is negation of bottom limit
        cout << "Y bounds: [" << bottomY << ", " << topY << "]" << endl;
        
        // Read Z limits
        configFile >> frontZ >> rearZ;
        cout << "Z bounds: [" << frontZ << ", " << rearZ << "]" << endl;
        
        configFile.close();
        cout << "Configuration read successfully!\n" << endl;
    }
    
    void initializePixelMapping() {
        cout << "Initializing pixel mapping..." << endl;
        
        // Calculate pixel step sizes
        dx = (rightX - leftX) / Screen_Width;
        dy = (topY - bottomY) / Screen_Height;
        
        // Calculate adjusted corner values
        Top_Y = topY - dy / 2.0;
        Left_X = leftX + dx / 2.0;
        
        cout << "Pixel mapping parameters:" << endl;
        cout << "dx = " << dx << endl;
        cout << "dy = " << dy << endl;
        cout << "Top_Y = " << Top_Y << endl;
        cout << "Left_X = " << Left_X << endl;
        cout << "Pixel mapping initialized!\n" << endl;
    }
    
    void initializeBuffers() {
        cout << "Initializing buffers..." << endl;
        
        // Initialize Z-buffer with z_max (rearZ) values
        zBuffer.resize(Screen_Height);
        for (int i = 0; i < Screen_Height; i++) {
            zBuffer[i].resize(Screen_Width, rearZ);
        }
        cout << "Z-buffer initialized with " << Screen_Height << " x " << Screen_Width << " resolution" << endl;
        cout << "All Z-buffer values set to z_max = " << rearZ << endl;
        
        // Initialize frame buffer (bitmap image) with black background
        frameBuffer = bitmap_image(Screen_Width, Screen_Height);
        frameBuffer.set_all_channels(0, 0, 0);  // Set background to black (RGB: 0,0,0)
        cout << "Frame buffer initialized with " << Screen_Width << " x " << Screen_Height << " resolution" << endl;
        cout << "Frame buffer background set to black" << endl;
        cout << "Buffers initialization complete!\n" << endl;
    }
    
    void readTriangles(const string& stage3Filename) {
        cout << "Reading triangles from: " << stage3Filename << endl;
        
        ifstream stage3File(stage3Filename);
        if (!stage3File) {
            cerr << "Error: Could not open " << stage3Filename << endl;
            exit(1);
        }
        
        string line;
        vector<Point4> points;
        
        // Read all points from stage3.txt
        while (getline(stage3File, line)) {
            if (line.empty()) {
                continue;  // Skip empty lines
            }
            
            istringstream iss(line);
            Point4 p;
            
            if (iss >> p.x >> p.y >> p.z) {
                p.w = 1.0;  // Set homogeneous coordinate
                points.push_back(p);
            }
        }
        
        stage3File.close();
        
        // Group points into triangles (every 3 consecutive points form a triangle)
        int numTriangles = points.size() / 3;
        triangles.resize(numTriangles);
        
        for (int i = 0; i < numTriangles; i++) {
            for (int j = 0; j < 3; j++) {
                triangles[i].points[j] = points[i * 3 + j];
            }
            // Random colors are already assigned in Triangle constructor
        }
        
        cout << "Read " << points.size() << " points forming " << numTriangles << " triangles" << endl;
        cout << "Triangles read successfully!\n" << endl;
    }
    
    void printDebugInfo() {
        cout << "=== DEBUG INFORMATION ===" << endl;
        cout << "\nConfiguration Parameters:" << endl;
        cout << "Screen Resolution: " << Screen_Width << " x " << Screen_Height << endl;
        cout << "X Range: [" << leftX << ", " << rightX << "]" << endl;
        cout << "Y Range: [" << bottomY << ", " << topY << "]" << endl;
        cout << "Z Range: [" << frontZ << ", " << rearZ << "]" << endl;
        
        cout << "\nTriangle Information:" << endl;
        cout << "Total Triangles: " << triangles.size() << endl;
        
        // Print details of first few triangles for debugging
        int maxTrianglesToPrint = min(3, (int)triangles.size());
        for (int i = 0; i < maxTrianglesToPrint; i++) {
            triangles[i].print(i + 1);
            cout << endl;
        }
        
        if (triangles.size() > 3) {
            cout << "... and " << (triangles.size() - 3) << " more triangles" << endl;
        }
        
        cout << "=========================" << endl;
    }
    
    void demonstratePixelMapping() {
        cout << "\n=== PIXEL MAPPING DEMONSTRATION ===" << endl;
        cout << "Showing sample pixel center coordinates:" << endl;
        
        // Show some sample pixel centers
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                double pixelCenterX = Left_X + col * dx;
                double pixelCenterY = Top_Y - row * dy;
                cout << "Pixel[" << row << "," << col << "] center: (" 
                     << fixed << setprecision(4) << pixelCenterX << ", " 
                     << pixelCenterY << ")" << endl;
            }
        }
        
        cout << "... (showing first 5x5 pixels only)" << endl;
        cout << "====================================" << endl;
    }
    
    // Helper function to find Y coordinate for a given X on a line between two points
    double interpolateY(const Point4& p1, const Point4& p2, double x) {
        if (abs(p2.x - p1.x) < 1e-9) return p1.y; // Vertical line
        return p1.y + (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x);
    }
    
    // Helper function to interpolate Z coordinate
    double interpolateZ(const Point4& p1, const Point4& p2, double x, double y) {
        // Use bilinear interpolation or simple linear interpolation along the line
        if (abs(p2.x - p1.x) < 1e-9 && abs(p2.y - p1.y) < 1e-9) return p1.z;
        
        if (abs(p2.x - p1.x) > abs(p2.y - p1.y)) {
            // Interpolate along X
            return p1.z + (p2.z - p1.z) * (x - p1.x) / (p2.x - p1.x);
        } else {
            // Interpolate along Y
            return p1.z + (p2.z - p1.z) * (y - p1.y) / (p2.y - p1.y);
        }
    }
    
    // Helper function to interpolate color
    Color interpolateColor(const Color& c1, const Color& c2, double t) {
        int R = (int)(c1.R + t * (c2.R - c1.R));
        int G = (int)(c1.G + t * (c2.G - c1.G));
        int B = (int)(c1.B + t * (c2.B - c1.B));
        return Color(R, G, B);
    }
    
    // Convert world coordinates to pixel coordinates
    int worldToPixelRow(double y) {
        return (int)((Top_Y - y) / dy);
    }
    
    int worldToPixelCol(double x) {
        return (int)((x - Left_X) / dx);
    }
    
    // Convert pixel coordinates to world coordinates
    double pixelToWorldY(int row) {
        return Top_Y - row * dy;
    }
    
    double pixelToWorldX(int col) {
        return Left_X + col * dx;
    }
    
    void rasterizeTriangles() {
        cout << "\nStarting triangle rasterization..." << endl;
        
        for (int t = 0; t < triangles.size(); t++) {
            cout << "Processing Triangle " << (t + 1) << "..." << endl;
            rasterizeTriangle(triangles[t]);
        }
        
        cout << "Triangle rasterization complete!" << endl;
    }
    
    void rasterizeTriangle(const MyTriangle& triangle) {
        // Get triangle points
        Point4 a = triangle.points[0];
        Point4 b = triangle.points[1];
        Point4 c = triangle.points[2];
        
        // Sort vertices by y-coordinate (top to bottom)
        if (a.y < b.y) swap(a, b);
        if (b.y < c.y) swap(b, c);
        if (a.y < b.y) swap(a, b);
        
        cout << "    Triangle vertices (sorted): (" << a.x << "," << a.y << "," << a.z << "), " 
             << "(" << b.x << "," << b.y << "," << b.z << "), "
             << "(" << c.x << "," << c.y << "," << c.z << ")" << endl;
        
        double bottom_scanline = max(c.y, bottomY);
        double top_scanline = min(a.y, topY);
        
        int top_row = round((top_scanline - bottomY) / dy);
        int bottom_row = round((bottom_scanline - bottomY) / dy);
        
        for (int i = top_row; i >= bottom_row; i--) {
            double y_s = bottomY + i * dy;
            
            // Find intersections with triangle edges
            double x_a, x_b, z_a, z_b;
            bool found_intersections = false;
            
            // Check if scanline intersects with edges
            if (y_s <= a.y && y_s >= b.y && abs(a.y - b.y) > 1e-9) {
                x_a = a.x - (a.x - b.x) * (a.y - y_s) / (a.y - b.y);
                z_a = a.z - (a.z - b.z) * (a.y - y_s) / (a.y - b.y);
                found_intersections = true;
            }
            
            if (y_s <= a.y && y_s >= c.y && abs(a.y - c.y) > 1e-9) {
                x_b = a.x - (a.x - c.x) * (a.y - y_s) / (a.y - c.y);
                z_b = a.z - (a.z - c.z) * (a.y - y_s) / (a.y - c.y);
                
                if (!found_intersections) {
                    x_a = x_b;
                    z_a = z_b;
                    found_intersections = true;
                } else {
                    // We have both intersections
                }
            }
            
            if (y_s <= b.y && y_s >= c.y && abs(b.y - c.y) > 1e-9) {
                double x_temp = b.x - (b.x - c.x) * (b.y - y_s) / (b.y - c.y);
                double z_temp = b.z - (b.z - c.z) * (b.y - y_s) / (b.y - c.y);
                
                if (!found_intersections) {
                    x_a = x_temp;
                    z_a = z_temp;
                    found_intersections = true;
                } else {
                    x_b = x_temp;
                    z_b = z_temp;
                }
            }
            
            if (!found_intersections) continue;
            
            // Make sure x_a is left and x_b is right
            if (x_a > x_b) {
                swap(x_a, x_b);
                swap(z_a, z_b);
            }
            
            int left_col = round((max(x_a, leftX) - leftX) / dx);
            int right_col = round((min(x_b, rightX) - leftX) / dx);
            
            for (int j = left_col; j <= right_col; j++) {
                double x_p = leftX + j * dx;
                double z_p = z_b - (z_b - z_a) * (x_b - x_p) / (x_b - x_a);
                
                if (z_p >= frontZ && z_p < zBuffer[Screen_Height - 1 - i][j]) {
                    zBuffer[Screen_Height - 1 - i][j] = z_p;
                    frameBuffer.set_pixel(j, Screen_Height - 1 - i, 
                                        triangle.triangleColor.R, 
                                        triangle.triangleColor.G, 
                                        triangle.triangleColor.B);
                }
            }
        }
    }
    
    void checkEdgeIntersection(const Point4& p1, const Point4& p2, double y, vector<double>& intersections) {
        // Check if horizontal line at y intersects edge from p1 to p2
        double y1 = p1.y, y2 = p2.y;
        double x1 = p1.x, x2 = p2.x;
        
        // Check if y is within the edge's y range
        if ((y >= min(y1, y2)) && (y <= max(y1, y2))) {
            if (abs(y2 - y1) < 1e-9) {
                // Horizontal edge
                if (abs(y - y1) < 1e-9) {
                    intersections.push_back(x1);
                    intersections.push_back(x2);
                }
            } else {
                // Calculate intersection x
                double x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                intersections.push_back(x);
            }
        }
    }
    
    double calculateZValue(const MyTriangle& triangle, double x, double y) {
        // Use barycentric coordinates to interpolate Z value
        Point4 p1 = triangle.points[0];
        Point4 p2 = triangle.points[1]; 
        Point4 p3 = triangle.points[2];
        
        // Calculate barycentric coordinates
        double denom = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
        if (abs(denom) < 1e-9) {
            // Degenerate triangle, return average Z
            return (p1.z + p2.z + p3.z) / 3.0;
        }
        
        double alpha = ((p2.y - p3.y) * (x - p3.x) + (p3.x - p2.x) * (y - p3.y)) / denom;
        double beta = ((p3.y - p1.y) * (x - p3.x) + (p1.x - p3.x) * (y - p3.y)) / denom;
        double gamma = 1.0 - alpha - beta;
        
        return alpha * p1.z + beta * p2.z + gamma * p3.z;
    }
    
    Color calculatePixelColor(const MyTriangle& triangle, double x, double y) {
        // Return the single color assigned to the entire triangle
        return triangle.triangleColor;
    }
    
    void saveOutput() {
        cout << "\nSaving output files..." << endl;
        
        // Save bitmap image
        string imageFilename = "output.bmp";
        frameBuffer.save_image(imageFilename);
        cout << "Saved frame buffer to: " << imageFilename << endl;
        
        // Save Z-buffer
        string zbufferFilename = "z_buffer.txt";
        ofstream zbufferFile(zbufferFilename);
        if (!zbufferFile) {
            cerr << "Error: Could not open " << zbufferFilename << " for writing" << endl;
            return;
        }
        
        zbufferFile << fixed << setprecision(6);
        
        // Save all rows - rendered values or empty lines
        for (int row = 0; row < Screen_Height; row++) {
            vector<double> rowValues;
            for (int col = 0; col < Screen_Width; col++) {
                if (zBuffer[row][col] < rearZ) {
                    rowValues.push_back(zBuffer[row][col]);
                }
            }
            
            if (!rowValues.empty()) {
                for (int i = 0; i < rowValues.size(); i++) {
                    if (i > 0) zbufferFile << "\t";
                    zbufferFile << rowValues[i];
                }
            }
            // Always write a newline, even for empty rows
            zbufferFile << endl;
        }
        
        zbufferFile.close();
        cout << "Saved Z-buffer to: " << zbufferFilename << endl;
        cout << "Output files saved successfully!" << endl;
    }
    
    void freeMemory() {
        cout << "\nFreeing memory..." << endl;
        
        // Clear vectors (STL automatically manages memory)
        triangles.clear();
        zBuffer.clear();
        
        // Frame buffer memory is managed by bitmap_image destructor
        cout << "Memory freed successfully!" << endl;
    }
    
    void process() {
        printDebugInfo();
        demonstratePixelMapping();
        
        cout << "\n" << string(50, '=') << endl;
        cout << "STARTING Z-BUFFER ALGORITHM" << endl;
        cout << string(50, '=') << endl;
        
        rasterizeTriangles();
        saveOutput();
        freeMemory();
        
        cout << "\n" << string(50, '=') << endl;
        cout << "STAGE 4 PROCESSING COMPLETE!" << endl;
        cout << string(50, '=') << endl;
    }
};

int main(int argc, char* argv[]) {
    // Seed random number generator for colors
    srand(time(0));
    
    // Fixed paths for test case 1
    string configFile = "Test Cases/4/config.txt";
    string stage3File = "Test Cases/4/stage3.txt";
    
    cout << "=== TASK 4: Z-Buffer Processing ===" << endl;
    cout << "Config file: " << configFile << endl;
    cout << "Stage3 file: " << stage3File << endl << endl;
    
    ZBufferProcessor processor(configFile, stage3File);
    processor.process();
    
    return 0;
}




