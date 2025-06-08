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
        file << x << " " << y << " " << z << endl;
    }

    void readFromFile(ifstream& file) {
        file >> x >> y >> z;
    }
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y, z - other.z);
    }
    
    Point cross(const Point& other) const {
        return Point(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    double length() const {
        return sqrt(x*x + y*y + z*z);
    }
    
    void normalize() {
        double len = length();
        if (len != 0) {
            x /= len;
            y /= len;
            z /= len;
        }
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

class ViewTransformer {
private:
    ifstream sceneFile;
    ifstream stage1File;
    ofstream outputFile;
    Matrix viewMatrix;
    
    Point eye, look, up;
    
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
        
        sceneFile >> look.x >> look.y >> look.z;
        
        sceneFile >> up.x >> up.y >> up.z;
        
        string dummy;
        getline(sceneFile, dummy);
        if (dummy.empty()) {
            getline(sceneFile, dummy);
        }
    }
    
    void calculateViewMatrix() {
        Point l = look - eye;
        l.normalize();
        
        Point r = l.cross(up);
        r.normalize();
        
        Point u = r.cross(l);
        
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
            Point p;
            iss >> p.x >> p.y >> p.z;
            
            Point transformed = p.transform(viewMatrix);
            
            transformed.writeToFile(outputFile);
        }
        
        cout << "Stage 2 transformation complete. Output written to stage2.txt." << endl;
    }
};

int main() {
    ViewTransformer transformer("scene.txt", "stage1.txt", "stage2.txt");
    transformer.transformPoints();
    return 0;
}
