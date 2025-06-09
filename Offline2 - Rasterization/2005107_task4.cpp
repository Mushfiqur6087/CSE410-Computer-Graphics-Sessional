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
};

struct MyTriangle {
    Point4 points[3];
    Color triangleColor;
    MyTriangle() {
        triangleColor = Color();
    }
};

class ZBufferProcessor {
private:
    int Screen_Width, Screen_Height;
    double leftX, rightX, bottomY, topY, frontZ, rearZ;
    double dx, dy, Top_Y, Left_X;
    vector<vector<double>> zBuffer;
    bitmap_image frameBuffer;
    vector<MyTriangle> triangles;

public:
    ZBufferProcessor(const string& configFilename, const string& stage3Filename) {
        readConfiguration(configFilename);
        initializePixelMapping();
        initializeBuffers();
        readTriangles(stage3Filename);
    }

    void readConfiguration(const string& configFilename) {
        ifstream configFile(configFilename);
        if (!configFile) exit(1);
        configFile >> Screen_Width >> Screen_Height;
        configFile >> leftX; rightX = -leftX;
        configFile >> bottomY; topY = -bottomY;
        configFile >> frontZ >> rearZ;
        configFile.close();
    }

    void initializePixelMapping() {
        dx = (rightX - leftX) / Screen_Width;
        dy = (topY - bottomY) / Screen_Height;
        Top_Y = topY - dy / 2.0;
        Left_X = leftX + dx / 2.0;
    }

    void initializeBuffers() {
        zBuffer.assign(Screen_Height, vector<double>(Screen_Width, rearZ));
        frameBuffer = bitmap_image(Screen_Width, Screen_Height);
        frameBuffer.set_all_channels(0, 0, 0);
    }

    void readTriangles(const string& stage3Filename) {
        ifstream stage3File(stage3Filename);
        if (!stage3File) exit(1);
        vector<Point4> points;
        string line;
        while (getline(stage3File, line)) {
            if (line.empty()) continue;
            istringstream iss(line);
            Point4 p; 
            if (iss >> p.x >> p.y >> p.z) {
                p.w = 1.0;
                points.push_back(p);
            }
        }
        stage3File.close();
        int numT = points.size() / 3;
        triangles.resize(numT);
        for (int i = 0; i < numT; i++) {
            for (int j = 0; j < 3; j++)
                triangles[i].points[j] = points[i*3 + j];
        }
    }

    int worldToPixelRow(double y)    { return int((Top_Y - y) / dy); }
    int worldToPixelCol(double x)    { return int((x - Left_X) / dx); }
    double pixelToWorldY(int row)    { return Top_Y - row * dy; }
    double pixelToWorldX(int col)    { return Left_X + col * dx; }

    void rasterizeTriangles() {
        for (auto& tri : triangles)
            rasterizeTriangle(tri);
    }

    void rasterizeTriangle(const MyTriangle& tri) {
        // sort vertices by descending y
        Point4 a = tri.points[0], b = tri.points[1], c = tri.points[2];
        if (a.y < b.y) swap(a,b);
        if (b.y < c.y) swap(b,c);
        if (a.y < b.y) swap(a,b);

        double top_scanline    = min(a.y, topY);
        double bottom_scanline = max(c.y, bottomY);

        // compute raw scanline indices
        int raw_top = round((top_scanline    - bottomY) / dy);
        int raw_bot = round((bottom_scanline - bottomY) / dy);
        // clamp into [0..Screen_Height-1]
        int top_row    = min(raw_top,    Screen_Height - 1);
        int bottom_row = max(raw_bot,    0);

        for (int i = top_row; i >= bottom_row; --i) {
            double y_s = bottomY + i * dy;
            // find intersections of this scanline with triangle edges
            vector<pair<double,double>> XZ;
            auto interp = [&](const Point4& p1, const Point4& p2){
                if (y_s >= min(p1.y,p2.y) && y_s <= max(p1.y,p2.y) && fabs(p1.y-p2.y)>1e-9) {
                    double t  = (y_s - p1.y)/(p2.y - p1.y);
                    double x  = p1.x + t*(p2.x - p1.x);
                    double z  = p1.z + t*(p2.z - p1.z);
                    XZ.emplace_back(x,z);
                }
            };
            interp(a,b); interp(a,c); interp(b,c);
            if (XZ.size() < 2) continue;
            if (XZ[0].first > XZ[1].first) swap(XZ[0], XZ[1]);

            int left_col  = max(0, min(Screen_Width-1, int(round((XZ[0].first - leftX)/dx))));
            int right_col = max(0, min(Screen_Width-1, int(round((XZ[1].first - leftX)/dx))));

            for (int j = left_col; j <= right_col; ++j) {
                double x_p = leftX + j * dx;
                // linear interp z between the two intersection points
                double z_p = XZ[0].second + (x_p - XZ[0].first)
                             * (XZ[1].second - XZ[0].second)
                             / (XZ[1].first - XZ[0].first);

                int row = Screen_Height - 1 - i;
                if (z_p >= frontZ && z_p < zBuffer[row][j]) {
                    zBuffer[row][j] = z_p;
                    frameBuffer.set_pixel(j, row,
                        tri.triangleColor.R,
                        tri.triangleColor.G,
                        tri.triangleColor.B);
                }
            }
        }
    }

    void saveOutput() {
        frameBuffer.save_image("my_output.bmp");
        ofstream zb("my_z_buffer.txt");
        zb << fixed << setprecision(6);
        for (auto& row : zBuffer) {
            bool any = false;
            for (double v : row) {
                if (v < rearZ) { zb << v << "\t"; any = true; }
            }
            zb << "\n";
        }
    }

    void process() {
        rasterizeTriangles();
        saveOutput();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) return 1;
    ZBufferProcessor proc(argv[1], argv[2]);
    proc.process();
    return 0;
}
