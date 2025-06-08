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

class ProjectionTransformer {
private:
    ifstream sceneFile;
    ifstream stage2File;
    ofstream outputFile;
    Matrix projectionMatrix;
    
    double fovY, aspectRatio, near, far;
    
public:
    ProjectionTransformer(const string& sceneFilename, const string& stage2Filename, const string& outputFilename) {
        sceneFile.open(sceneFilename);
        if (!sceneFile) {
            cerr << "Error: Could not open " << sceneFilename << endl;
            exit(1);
        }
        
        stage2File.open(stage2Filename);
        if (!stage2File) {
            cerr << "Error: Could not open " << stage2Filename << endl;
            exit(1);
        }
        
        outputFile.open(outputFilename);
        if (!outputFile) {
            cerr << "Error: Could not open " << outputFilename << " for writing" << endl;
            exit(1);
        }
        
        outputFile << fixed << setprecision(7);
        
        readProjectionParameters();
        calculateProjectionMatrix();
    }
    
    ~ProjectionTransformer() {
        if (sceneFile.is_open()) sceneFile.close();
        if (stage2File.is_open()) stage2File.close();
        if (outputFile.is_open()) outputFile.close();
    }
    
    void readProjectionParameters() {
        string dummy;
        for (int i = 0; i < 3; i++) {
            getline(sceneFile, dummy);
        }
        sceneFile >> fovY >> aspectRatio >> near >> far;
        fovY = fovY * M_PI / 180.0;
    }
    
    void calculateProjectionMatrix() {
        double fovX = fovY * aspectRatio;
        double t = near * tan(fovY / 2);
        double r = near * tan(fovX / 2);
        
        projectionMatrix = Matrix(); 
        
        projectionMatrix.values[0][0] = near / r;
        projectionMatrix.values[1][1] = near / t;
        projectionMatrix.values[2][2] = -(far + near) / (far - near);
        projectionMatrix.values[2][3] = -(2 * far * near) / (far - near);
        projectionMatrix.values[3][2] = -1;
        projectionMatrix.values[3][3] = 0;
    }
    
    void transformPoints() {
        string line;
        
        while (getline(stage2File, line)) {
            if (line.empty()) {
                outputFile << endl;
                continue;
            }
            
            istringstream iss(line);
            Point4 p;
            iss >> p.x >> p.y >> p.z;
            p.w = 1.0; // Ensure it's a point
            
            Point4 transformed = p * projectionMatrix;
            
            transformed.writeToFile(outputFile);
        }
        
        cout << "Stage 3 transformation complete. Output written to stage3.txt." << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <scene_file> <input_stage2_file> <output_stage3_file>" << endl;
        return 1;
    }
    
    ProjectionTransformer transformer(argv[1], argv[2], argv[3]);
    transformer.transformPoints();
    return 0;
}
