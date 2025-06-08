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
class Point4;
class Vector4;

Matrix createTranslationMatrix(double tx, double ty, double tz);
Matrix createScalingMatrix(double sx, double sy, double sz);
Matrix createRotationMatrix(double angle, double ax, double ay, double az);
Vector4 rotateVectorRodrigues(const Vector4& v, const Vector4& axis, double angle);

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

class Vector4 {
public:
    double x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}
    Vector4(double x, double y, double z) : x(x), y(y), z(z), w(0) {} // Vector constructor

    Vector4 operator*(const Matrix& matrix) const {
        double x_new = matrix.values[0][0] * x + matrix.values[0][1] * y + matrix.values[0][2] * z + matrix.values[0][3] * w;
        double y_new = matrix.values[1][0] * x + matrix.values[1][1] * y + matrix.values[1][2] * z + matrix.values[1][3] * w;
        double z_new = matrix.values[2][0] * x + matrix.values[2][1] * y + matrix.values[2][2] * z + matrix.values[2][3] * w;
        double w_new = matrix.values[3][0] * x + matrix.values[3][1] * y + matrix.values[3][2] * z + matrix.values[3][3] * w;
        return Vector4(x_new, y_new, z_new, w_new);
    }

    Vector4 operator-(const Vector4& other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Vector4 cross(const Vector4& other) const {
        return Vector4(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x,
            0 // Cross product result is always a vector
        );
    }

    double dot(const Vector4& other) const {
        return x * other.x + y * other.y + z * other.z; // Don't include w for 3D dot product
    }

    double length() const {
        return sqrt(x*x + y*y + z*z); // Don't include w for 3D length
    }

    void normalize() {
        double len = length();
        if (len != 0) {
            x /= len;
            y /= len;
            z /= len;
            // w remains unchanged for vectors (should be 0)
        }
    }
};

class Point4 {
public:
    double x, y, z, w;

    Point4() : x(0), y(0), z(0), w(1) {}
    Point4(double x, double y, double z) : x(x), y(y), z(z), w(1) {} // Point constructor
    Point4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

    Point4 operator*(const Matrix& matrix) const {
        double x_new = matrix.values[0][0] * x + matrix.values[0][1] * y + matrix.values[0][2] * z + matrix.values[0][3] * w;
        double y_new = matrix.values[1][0] * x + matrix.values[1][1] * y + matrix.values[1][2] * z + matrix.values[1][3] * w;
        double z_new = matrix.values[2][0] * x + matrix.values[2][1] * y + matrix.values[2][2] * z + matrix.values[2][3] * w;
        double w_new = matrix.values[3][0] * x + matrix.values[3][1] * y + matrix.values[3][2] * z + matrix.values[3][3] * w;
        
        // Normalize if w != 1 (perspective division)
        if (w_new != 1.0 && w_new != 0.0) {
            x_new /= w_new;
            y_new /= w_new;
            z_new /= w_new;
            w_new = 1.0;
        }
        
        return Point4(x_new, y_new, z_new, w_new);
    }

    Vector4 operator-(const Point4& other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, 0); // Point - Point = Vector
    }
    
    void writeToFile(ofstream& file) const {
        file << fixed << setprecision(7) << x << " " << y << " " << z << endl;
    }

    void readFromFile(ifstream& file) {
        file >> x >> y >> z;
        w = 1.0; // Ensure it's a point
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
    
    // Normalize the rotation axis
    double length = sqrt(ax*ax + ay*ay + az*az);
    if (length != 0) {
        ax /= length;
        ay /= length;
        az /= length;
    }
    
    Vector4 axis(ax, ay, az, 0); // Use Vector4 for axis
    
    // Define the standard unit vectors i, j, k as vectors
    Vector4 i(1.0, 0.0, 0.0, 0);
    Vector4 j(0.0, 1.0, 0.0, 0);
    Vector4 k(0.0, 0.0, 1.0, 0);
    
    // Transform each unit vector using Rodrigues' formula
    Vector4 i_rotated = rotateVectorRodrigues(i, axis, angle);
    Vector4 j_rotated = rotateVectorRodrigues(j, axis, angle);
    Vector4 k_rotated = rotateVectorRodrigues(k, axis, angle);
    
    // Build the rotation matrix from the transformed unit vectors
    // The first column is the transformed i vector
    matrix.values[0][0] = i_rotated.x;
    matrix.values[1][0] = i_rotated.y;
    matrix.values[2][0] = i_rotated.z;
    
    // The second column is the transformed j vector
    matrix.values[0][1] = j_rotated.x;
    matrix.values[1][1] = j_rotated.y;
    matrix.values[2][1] = j_rotated.z;
    
    // The third column is the transformed k vector
    matrix.values[0][2] = k_rotated.x;
    matrix.values[1][2] = k_rotated.y;
    matrix.values[2][2] = k_rotated.z;
    
    // The translation part remains zero for pure rotation
    matrix.values[0][3] = 0.0;
    matrix.values[1][3] = 0.0;
    matrix.values[2][3] = 0.0;
    
    // Bottom row for homogeneous coordinates
    matrix.values[3][0] = 0.0;
    matrix.values[3][1] = 0.0;
    matrix.values[3][2] = 0.0;
    matrix.values[3][3] = 1.0;
    
    return matrix;
}

// Helper function to rotate a vector around an axis using Rodrigues' formula
Vector4 rotateVectorRodrigues(const Vector4& v, const Vector4& axis, double angle) {
    double rad = angle * M_PI / 180.0;
    double cosTheta = cos(rad);
    double sinTheta = sin(rad);
    
    // v_rot = v*cos(θ) + (k × v)*sin(θ) + k*(k · v)*(1 - cos(θ))
    // where k is the normalized rotation axis and v is the vector to rotate
    
    // k · v (dot product)
    double dotProduct = axis.dot(v);
    
    // k × v (cross product)
    Vector4 crossProduct = axis.cross(v);
    
    // Apply Rodrigues' formula
    Vector4 rotated(
        v.x * cosTheta + crossProduct.x * sinTheta + axis.x * dotProduct * (1.0 - cosTheta),
        v.y * cosTheta + crossProduct.y * sinTheta + axis.y * dotProduct * (1.0 - cosTheta),
        v.z * cosTheta + crossProduct.z * sinTheta + axis.z * dotProduct * (1.0 - cosTheta),
        0 // Result is a vector
    );
    
    return rotated;
}

class Triangle {
public:
    Point4 p1, p2, p3;
    
    Triangle() {}
    
    void readFromFile(ifstream& file) {
        p1.readFromFile(file);
        p2.readFromFile(file);
        p3.readFromFile(file);
    }
    
    void transform(const Matrix& matrix) {
        p1 = p1 * matrix; // Use matrix multiplication directly
        p2 = p2 * matrix;
        p3 = p3 * matrix;
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
