#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>

using namespace std;

class Matrix;

Matrix createTranslationMatrix(double tx, double ty, double tz);
Matrix createScalingMatrix(double sx, double sy, double sz);
Matrix createRotationMatrix(double angle, double ax, double ay, double az);

class Matrix {
public:
    double values[4][4];

    Matrix() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                values[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
    }

    Matrix operator*(const Matrix& other) const {
        Matrix result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.values[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.values[i][j] += values[i][k] * other.values[k][j];
                }
            }
        }
        return result;
    }

    void print() const {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << values[i][j] << " ";
            }
            cout << endl;
        }
    }
};

Matrix createTranslationMatrix(double tx, double ty, double tz) {
    Matrix matrix;
    matrix.values[0][3] = tx;
    matrix.values[1][3] = ty;
    matrix.values[2][3] = tz;
    return matrix;
}

Matrix createScalingMatrix(double sx, double sy, double sz) {
    Matrix matrix;
    matrix.values[0][0] = sx;
    matrix.values[1][1] = sy;
    matrix.values[2][2] = sz;
    return matrix;
}

Matrix createRotationMatrix(double angle, double ax, double ay, double az) {
    Matrix matrix;
    
    double length = sqrt(ax*ax + ay*ay + az*az);
    if (length != 0) {
        ax /= length;
        ay /= length;
        az /= length;
    }
    
    double rad = angle * M_PI / 180.0;
    double c = cos(rad);
    double s = sin(rad);
    double t = 1.0 - c;
    
    matrix.values[0][0] = t*ax*ax + c;
    matrix.values[0][1] = t*ax*ay - s*az;
    matrix.values[0][2] = t*ax*az + s*ay;
    
    matrix.values[1][0] = t*ax*ay + s*az;
    matrix.values[1][1] = t*ay*ay + c;
    matrix.values[1][2] = t*ay*az - s*ax;
    
    matrix.values[2][0] = t*ax*az - s*ay;
    matrix.values[2][1] = t*ay*az + s*ax;
    matrix.values[2][2] = t*az*az + c;
    
    return matrix;
}

class Point {
public:
    double x, y, z;

    Point() : x(0), y(0), z(0) {}
    Point(double x, double y, double z) : x(x), y(y), z(z) {}

    Point transform(const Matrix& matrix) const {
        double x_new = matrix.values[0][0] * x + matrix.values[0][1] * y + matrix.values[0][2] * z + matrix.values[0][3];
        double y_new = matrix.values[1][0] * x + matrix.values[1][1] * y + matrix.values[1][2] * z + matrix.values[1][3];
        double z_new = matrix.values[2][0] * x + matrix.values[2][1] * y + matrix.values[2][2] * z + matrix.values[2][3];
        double w = matrix.values[3][0] * x + matrix.values[3][1] * y + matrix.values[3][2] * z + matrix.values[3][3];
        
        if (w != 1.0 && w != 0.0) {
            x_new /= w;
            y_new /= w;
            z_new /= w;
        }
        
        return Point(x_new, y_new, z_new);
    }
    
    void writeToFile(ofstream& file) const {
        file << x << " " << y << " " << z << endl;
    }

    void readFromFile(ifstream& file) {
        file >> x >> y >> z;
    }
};

class Triangle {
public:
    Point p1, p2, p3;
    
    Triangle() {}
    
    void readFromFile(ifstream& file) {
        p1.readFromFile(file);
        p2.readFromFile(file);
        p3.readFromFile(file);
    }
    
    void transform(const Matrix& matrix) {
        p1 = p1.transform(matrix);
        p2 = p2.transform(matrix);
        p3 = p3.transform(matrix);
    }
    
    void writeToFile(ofstream& file) const {
        p1.writeToFile(file);
        p2.writeToFile(file);
        p3.writeToFile(file);
        file << endl;
    }
};

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
        
        Point p1_transformed = triangle.p1.transform(transformStack.getCurrentTransform());
        Point p2_transformed = triangle.p2.transform(transformStack.getCurrentTransform());
        Point p3_transformed = triangle.p3.transform(transformStack.getCurrentTransform());
        
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

int main() {
    SceneProcessor processor("scene.txt", "stage1.txt");
    processor.processScene();
    return 0;
}