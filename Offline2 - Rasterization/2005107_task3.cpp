#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <cmath>
#include <iomanip>
#include <vector>
#include <sstream>

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
        file << fixed << setprecision(7) << x << " " << y << " " << z << endl;
    }

    void readFromFile(ifstream& file) {
        file >> x >> y >> z;
    }
};

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
            Point p;
            iss >> p.x >> p.y >> p.z;
            
            Point transformed = p.transform(projectionMatrix);
            
            transformed.writeToFile(outputFile);
        }
        
        cout << "Stage 3 transformation complete. Output written to stage3.txt." << endl;
    }
};

int main() {
    ProjectionTransformer transformer("scene.txt", "stage2.txt", "stage3.txt");
    transformer.transformPoints();
    return 0;
}
