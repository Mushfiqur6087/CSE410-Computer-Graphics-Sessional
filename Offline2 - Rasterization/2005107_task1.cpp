#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>
#include "helper.h"

using namespace std;

class TransformationStack {
private:
    stack<Matrix> matrices;
    
public:
    TransformationStack() {
        matrices.push(Matrix());
    }
    
    void push() {
        matrices.push(matrices.top());
    }
    
    void pop() {
        if (matrices.size() > 1) {
            matrices.pop();
        }
    }
    
    void applyTranslation(double tx, double ty, double tz) {
        Matrix translationMatrix = createTranslationMatrix(tx, ty, tz);
        Matrix newTop = matrices.top() * translationMatrix;
        matrices.pop();
        matrices.push(newTop);
    }
    
    void applyScaling(double sx, double sy, double sz) {
        Matrix scalingMatrix = createScalingMatrix(sx, sy, sz);
        Matrix newTop = matrices.top() * scalingMatrix;
        matrices.pop();
        matrices.push(newTop);
    }
    
    void applyRotation(double angle, double ax, double ay, double az) {
        Matrix rotationMatrix = createRotationMatrix(angle, ax, ay, az);
        Matrix newTop = matrices.top() * rotationMatrix;
        matrices.pop();
        matrices.push(newTop);
    }
    
    Matrix getCurrentTransform() const {
        return matrices.top();
    }
};

class SceneProcessor {
private:
    ifstream sceneFile;
    ofstream outputFile;
    TransformationStack transformStack;
    
public:
    SceneProcessor(const string& inputFilename, const string& outputFilename) {
        sceneFile.open(inputFilename);
        if (!sceneFile) {
            cerr << "Error: Could not open " << inputFilename << endl;
            exit(1);
        }
        
        outputFile.open(outputFilename);
        if (!outputFile) {
            cerr << "Error: Could not open " << outputFilename << " for writing" << endl;
            exit(1);
        }
        
        outputFile << fixed << setprecision(7);
    }
    
    ~SceneProcessor() {
        if (sceneFile.is_open()) sceneFile.close();
        if (outputFile.is_open()) outputFile.close();
    }
    
    void skipCameraParameters() {
        string dummy;
        for (int i = 0; i < 4; i++) {
            getline(sceneFile, dummy);
        }
    }
    
    void processScene() {
        skipCameraParameters();
        
        string command;
        while (sceneFile >> command) {
            if (command == "triangle") {
                processTriangle();
            } 
            else if (command == "translate") {
                processTranslate();
            } 
            else if (command == "scale") {
                processScale();
            } 
            else if (command == "rotate") {
                processRotate();
            } 
            else if (command == "push") {
                transformStack.push();
            } 
            else if (command == "pop") {
                transformStack.pop();
            } 
            else if (command == "end") {
                break;
            }
        }
        
        cout << "Stage 1 transformation complete. Output written to stage1.txt." << endl;
    }
    
private:
    void processTriangle() {
        Triangle triangle;
        triangle.readFromFile(sceneFile);
        
        // Transform each point using matrix multiplication
        Point4 p1_transformed = triangle.p1 * transformStack.getCurrentTransform();
        Point4 p2_transformed = triangle.p2 * transformStack.getCurrentTransform();
        Point4 p3_transformed = triangle.p3 * transformStack.getCurrentTransform();
        
        p1_transformed.writeToFile(outputFile);
        p2_transformed.writeToFile(outputFile);
        p3_transformed.writeToFile(outputFile);
        outputFile << endl;
    }
    
    void processTranslate() {
        double tx, ty, tz;
        sceneFile >> tx >> ty >> tz;
        transformStack.applyTranslation(tx, ty, tz);
    }
    
    void processScale() {
        double sx, sy, sz;
        sceneFile >> sx >> sy >> sz;
        transformStack.applyScaling(sx, sy, sz);
    }
    
    void processRotate() {
        double angle, ax, ay, az;
        sceneFile >> angle >> ax >> ay >> az;
        transformStack.applyRotation(angle, ax, ay, az);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input_scene_file> <output_stage1_file>" << endl;
        return 1;
    }
    
    SceneProcessor processor(argv[1], argv[2]);
    processor.processScene();
    return 0;
}