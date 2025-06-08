#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>
#include <sstream>
#include "helper.h"

using namespace std;

class ViewTransformer {
private:
    ifstream sceneFile;
    ifstream stage1File;
    ofstream outputFile;
    Matrix viewMatrix;
    
    Point4 eye, look, up;
    
public:
    ViewTransformer(const string& sceneFilename, const string& stage1Filename, const string& outputFilename) {
        sceneFile.open(sceneFilename);
        if (!sceneFile) {
            cerr << "Error: Could not open " << sceneFilename << endl;
            exit(1);
        }
        
        stage1File.open(stage1Filename);
        if (!stage1File) {
            cerr << "Error: Could not open " << stage1Filename << endl;
            exit(1);
        }
        
        outputFile.open(outputFilename);
        if (!outputFile) {
            cerr << "Error: Could not open " << outputFilename << " for writing" << endl;
            exit(1);
        }
        
        outputFile << fixed << setprecision(7);
        
        readCameraParameters();
        calculateViewMatrix();
    }
    
    ~ViewTransformer() {
        if (sceneFile.is_open()) sceneFile.close();
        if (stage1File.is_open()) stage1File.close();
        if (outputFile.is_open()) outputFile.close();
    }
    
    void readCameraParameters() {
        sceneFile >> eye.x >> eye.y >> eye.z;
        eye.w = 1.0; // Ensure it's a point
        
        sceneFile >> look.x >> look.y >> look.z;
        look.w = 1.0; // Ensure it's a point
        
        sceneFile >> up.x >> up.y >> up.z;
        up.w = 0.0; // Up is a vector
        
        string dummy;
        getline(sceneFile, dummy);
        if (dummy.empty()) {
            getline(sceneFile, dummy);
        }
    }
    
    void calculateViewMatrix() {
        Vector4 l = look - eye; // Point - Point = Vector
        l.normalize();
        
        Vector4 upVec(up.x, up.y, up.z, 0); // Convert to vector
        Vector4 r = l.cross(upVec);
        r.normalize();
        
        Vector4 u = r.cross(l);
        
        Matrix T;
        T.values[0][3] = -eye.x;
        T.values[1][3] = -eye.y;
        T.values[2][3] = -eye.z;
        
        Matrix R;
        R.values[0][0] = r.x;  R.values[0][1] = r.y;  R.values[0][2] = r.z;
        R.values[1][0] = u.x;  R.values[1][1] = u.y;  R.values[1][2] = u.z;
        R.values[2][0] = -l.x; R.values[2][1] = -l.y; R.values[2][2] = -l.z;
        
        viewMatrix = R * T;
    }
    
    void transformPoints() {
        string line;
        int lineCount = 0;
        
        while (getline(stage1File, line)) {
            if (line.empty()) {
                outputFile << endl;
                continue;
            }
            
            istringstream iss(line);
            Point4 p;
            iss >> p.x >> p.y >> p.z;
            p.w = 1.0; // Ensure it's a point
            
            Point4 transformed = p * viewMatrix;
            
            transformed.writeToFile(outputFile);
        }
        
        cout << "Stage 2 transformation complete. Output written to stage2.txt." << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <scene_file> <input_stage1_file> <output_stage2_file>" << endl;
        return 1;
    }
    
    ViewTransformer transformer(argv[1], argv[2], argv[3]);
    transformer.transformPoints();
    return 0;
}
