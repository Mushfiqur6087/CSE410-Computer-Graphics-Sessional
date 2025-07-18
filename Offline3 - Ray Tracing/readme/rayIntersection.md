# Ray Intersection Detailed Guide

This document explains how ray intersection is calculated for different objects in our ray tracer, including spheres and planes. We'll break down the mathematical formulas and their implementation.

## Ray Representation

A ray is represented by its origin point (O) and direction vector (D):
```
Ray(t) = O + t × D
```
where:
- O = Origin point (x₀, y₀, z₀)
- D = Direction vector (normalized) (dx, dy, dz)
- t = Distance parameter along the ray

## 1. Sphere Intersection

### Mathematical Foundation

For a sphere with center C and radius r, any point P on the sphere's surface satisfies:
```
|P - C|² = r²
```

Substituting the ray equation:
```
|O + tD - C|² = r²
```

Expanding this equation:
```
(O + tD - C)·(O + tD - C) = r²
(tD)·(tD) + 2(O - C)·(tD) + (O - C)·(O - C) = r²

t²(D·D) + 2t(O - C)·D + |O - C|² - r² = 0
```

Since D is normalized, D·D = 1, giving us:
```
t² + 2t((O - C)·D) + |O - C|² - r² = 0
```

Let L = O - C (vector from sphere center to ray origin)
```
t² + 2t(L·D) + |L|² - r² = 0
```

This is a quadratic equation in standard form: at² + bt + c = 0
where:
- a = 1
- b = 2(L·D)
- c = |L|² - r²

### Implementation Analysis

From your implementation in `2.1 Test Against Red Sphere`, we see:

```
Sphere Center: C = (2, 1, 0)
Sphere Radius: r = 1.5
Ray Origin: O = (0, 0, 10)
Ray Direction: D = (0.188, 0.094, -0.978)

Vector from origin to center: L = C - O = (2, 1, -10)

Projection of L onto D: t_ca = L·D
t_ca = (2, 1, -10)·(0.188, 0.094, -0.978)
t_ca = 10.25

Distance squared from sphere center to ray: d² = |L|² - t_ca²
|L|² = 105
d² = 105 - 105.06 = -0.06
```

The intersection points are calculated as:
```
t₁ = t_ca - t_hc = 10.25 - 1.52 = 8.73
t₂ = t_ca + t_hc = 10.25 + 1.52 = 11.77
```

### Intersection Cases

1. If d² > r², ray misses sphere (no intersection)
2. If d² = r², ray grazes sphere (one intersection)
3. If d² < r², ray penetrates sphere (two intersections)

## 2. Plane Intersection

### Mathematical Foundation

A plane is defined by a normal vector N and a point P₀ on the plane (or distance d from origin). Any point P on the plane satisfies:
```
N·(P - P₀) = 0
```
or in distance form:
```
N·P + d = 0
```

Substituting the ray equation:
```
N·(O + tD) + d = 0
N·O + t(N·D) + d = 0
```

Solving for t:
```
t = -(N·O + d)/(N·D)
```

### Implementation Analysis

From your implementation in `2.3 Test Against Green Plane`:

```
Plane Normal: N = (0, 1, 0)
Plane Distance: d = -3
Ray: O = (0, 0, 10), D = (0.188, 0.094, -0.978)

Plane equation: N·P + d = 0
(0, 1, 0)·P - 3 = 0, meaning P_y = 3

Ray equation: P = O + t × D
P = (0.188t, 0.094t, 10 - 0.978t)

For intersection: P_y = 3
0.094t = 3
t = 31.91
```

### Intersection Cases

1. If N·D = 0:
   - If N·O + d = 0: Ray lies in plane (infinite intersections)
   - Otherwise: No intersection (ray parallel to plane)
2. If N·D ≠ 0: One intersection point

## 6. Intersection Testing Algorithm

Based on your actual implementation, here's the complete intersection testing flow:

```cpp
// Main ray tracing loop - from your implementation
for (int i = 0; i < imageWidth; i++) {
    for (int j = 0; j < imageHeight; j++) {
        // Generate ray for pixel (i, j)
        Ray ray = generateRay(i, j);
        
        // Find closest intersection
        double minT = infinity;
        Object* closestObject = nullptr;
        
        for (Object* obj : objects) {
            double t = obj->intersect(&ray);
            if (t > 0 && t < minT) {
                minT = t;
                closestObject = obj;
            }
        }
        
        // Calculate lighting if intersection found
        if (closestObject != nullptr) {
            Color pixelColor;
            closestObject->phongLighting(&ray, &pixelColor, 0);
            setPixel(i, j, pixelColor);
        }
    }
}
```

### Object-Specific Intersection Methods:

```cpp
// Sphere intersection - your implementation
double Sphere::intersect(Ray *ray) {
    Vector3D originalOrigin = ray->getOrigin();
    ray->setOrigin(ray->getOrigin() - referencePoint);  // Transform to sphere space
    
    double a = 1;  // ray direction is normalized
    double b = 2 * (ray->getDirection() * ray->getOrigin());
    double c = ray->getOrigin() * ray->getOrigin() - length * length;
    
    double discriminant = b * b - 4 * a * c;
    ray->setOrigin(originalOrigin);  // Restore original ray
    
    if (discriminant < 0) return -1.0;
    
    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);
    
    // Return closest positive intersection
    if (t1 > 0 && t2 > 0) return min(t1, t2);
    else if (t1 > 0) return t1;
    else if (t2 > 0) return t2;
    else return -1.0;
}

// Triangle intersection - your implementation  
double Triangle::intersect(Ray *ray) {
    Vector3D normal = computeNormal(referencePoint);
    double denominator = normal * ray->getDirection();
    
    if (fabs(denominator) < epsilon) return -1.0;  // Parallel ray
    
    double t = ((referencePoint - ray->getOrigin()) * normal) / denominator;
    if (t < 0) return -1.0;  // Behind ray origin
    
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * t;
    
    // Inside-outside test using cross products
    Vector3D crossProduct0 = (vertexB - vertexA) ^ (intersectionPoint - vertexA);
    Vector3D crossProduct1 = (vertexC - vertexB) ^ (intersectionPoint - vertexB);
    Vector3D crossProduct2 = (vertexA - vertexC) ^ (intersectionPoint - vertexC);
    
    if ((crossProduct0 * crossProduct1 >= 0) && (crossProduct1 * crossProduct2 >= 0)) {
        return t;
    }
    
    return -1.0;
}

// Floor intersection - your implementation
double Floor::intersect(Ray *ray) {
    double t = (height - ray->getOrigin().z) / ray->getDirection().z;
    if (t < 0) return -1.0;  // Behind ray origin
    
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * t;
    
    // Check bounds
    if (intersectionPoint.x < referencePoint.x || 
        intersectionPoint.x > referencePoint.x + width ||
        intersectionPoint.y < referencePoint.y || 
        intersectionPoint.y > referencePoint.y + length) {
        return -1.0;  // Outside floor bounds
    }
    
    return t;
}
```

## 4. Performance Considerations

### Sphere Intersection Optimization
- Early exit if sphere is behind ray (t_ca < 0)
- Early exit if ray misses sphere (d² > r²)
- Avoid square root until necessary

### Plane Intersection Optimization
- Early exit if ray is parallel to plane (N·D ≈ 0)
- Only one dot product needed for intersection test

### General Optimizations
- Use axis-aligned bounding boxes for complex objects
- Sort objects by approximate distance from camera
- Implement spatial partitioning for many objects

## 5. Example Intersection Results

From your scene with a red sphere, blue sphere, and green plane:

```
Red Sphere (2, 1, 0), r=1.5:
- Hit at t = 8.73
- Intersection point: (1.641, 0.821, 1.462)

Blue Sphere (-1, -1, -2), r=1.0:
- No intersection (d² > r²)

Green Plane at y=3:
- Hit at t = 31.91
- Intersection point: (6.00, 3, -21.21)
```

This demonstrates how the closest intersection (red sphere at t=8.73) is chosen for further lighting calculations.

## 6. Implementation Details from Your Code

### Base Object Class
Your implementation uses an abstract `Object` class that defines the interface for all intersectable objects:

```cpp
class Object {
    // Base class for all objects that can be intersected by rays
    virtual double intersect(Ray *ray) = 0;
    virtual Vector3D computeNormal(Vector3D point) = 0;
    // ... other methods for phong lighting, etc.
};
```

### Sphere Implementation
Your sphere intersection is implemented in `Sphere::intersect`:

```cpp
double Sphere::intersect(Ray *ray) {
    // Transform ray origin relative to sphere center
    Vector3D originalOrigin = ray->getOrigin();
    ray->setOrigin(ray->getOrigin() - referencePoint);
    
    // Compute quadratic equation coefficients
    double a = 1;  // Since ray direction is normalized
    double b = 2 * (ray->getDirection() * ray->getOrigin());
    double c = ray->getOrigin() * ray->getOrigin() - length * length;
    
    // Solve quadratic equation
    double discriminant = b * b - 4 * a * c;
    
    // Reset ray origin
    ray->setOrigin(originalOrigin);
    
    if (discriminant < 0) return -1.0;
    
    // Find closest positive intersection
    double t1 = (-b + sqrt(discriminant)) / (2 * a);
    double t2 = (-b - sqrt(discriminant)) / (2 * a);
    
    return chooseClosestPositiveT(t1, t2);
}
```

### Triangle Implementation
Your triangle intersection uses the plane intersection plus inside-triangle test:

```cpp
double Triangle::intersect(Ray *ray) {
    // Compute triangle normal
    Vector3D normal = computeNormal(referencePoint);
    double denominator = normal * ray->getDirection();
    
    // Check if ray is parallel to triangle plane
    if (fabs(denominator) < epsilon) return -1.0;
    
    // Find intersection with triangle's plane
    double t = ((referencePoint - ray->getOrigin()) * normal) / denominator;
    if (t < 0) return -1.0;
    
    // Compute intersection point
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * t;
    
    // Inside-outside test using cross products
    Vector3D crossProduct0 = (vertexB - vertexA) ^ (intersectionPoint - vertexA);
    Vector3D crossProduct1 = (vertexC - vertexB) ^ (intersectionPoint - vertexB);
    Vector3D crossProduct2 = (vertexA - vertexC) ^ (intersectionPoint - vertexC);
    
    // Check if point is inside triangle
    if ((crossProduct0 * crossProduct1 >= 0) && (crossProduct1 * crossProduct2 >= 0)) {
        return t;
    }
    
    return -1.0;
}
```

## 3. Triangle Intersection: Mathematical Details

### 3.1 Mathematical Foundation
Triangle intersection is a two-step process:
1. **Plane Intersection**: Find where the ray intersects the plane containing the triangle
2. **Inside Test**: Determine if the intersection point is inside the triangle

#### Step 1: Plane Intersection
Your triangle normal is computed using the cross product of two edges:
```cpp
Vector3D Triangle::computeNormal(Vector3D point) {
    Vector3D normal = (vertexB - vertexA) ^ (vertexC - vertexA);
    normal.normalize();
    return normal;
}
```

This creates a normal vector **N** = (B - A) × (C - A), where A, B, C are the triangle vertices.

#### Step 2: Inside-Outside Test
Your implementation uses the **same-side method** with cross products:

```
For each edge of the triangle:
1. Compute cross product of edge vector with vector from edge start to intersection point
2. Check if all cross products have the same orientation (same sign when dot product taken)
```

### 3.2 Cross Product Inside Test Explained
```cpp
// For triangle ABC and intersection point P
Vector3D edge1 = B - A;  // First edge
Vector3D edge2 = C - B;  // Second edge  
Vector3D edge3 = A - C;  // Third edge

Vector3D C1 = edge1 ^ (P - A);  // Cross product for edge AB
Vector3D C2 = edge2 ^ (P - B);  // Cross product for edge BC
Vector3D C3 = edge3 ^ (P - C);  // Cross product for edge CA

// Point is inside if all cross products have same orientation
if ((C1 * C2 >= 0) && (C2 * C3 >= 0)) return t;
```

The dot product check `(C1 * C2 >= 0)` ensures that the cross products point in the same direction, meaning the point is on the same side of all edges.

### 3.3 Alternative Triangle Intersection Methods
Your implementation could be extended with other methods:

1. **Barycentric Coordinates**: More numerically stable
2. **Möller-Trumbore Algorithm**: More efficient for ray tracing
3. **Edge Function Method**: Better for rasterization

## 4. Floor Object Implementation

### 4.1 Floor as Specialized Plane
Your `Floor` class extends the basic plane intersection with spatial bounds:

```cpp
double Floor::intersect(Ray *ray) {
    // Intersect with horizontal plane at height z = height
    double t = (height - ray->getOrigin().z) / ray->getDirection().z;
    if (t < 0) return -1.0;  // Behind ray origin
    
    // Check if intersection point is within floor bounds
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * t;
    if (intersectionPoint.x < referencePoint.x || 
        intersectionPoint.x > referencePoint.x + width ||
        intersectionPoint.y < referencePoint.y || 
        intersectionPoint.y > referencePoint.y + length) {
        return -1.0;  // Outside floor bounds
    }
    
    return t;
}
```

### 4.2 Advanced Floor Features
Your floor implementation includes:

1. **Checkerboard Pattern**: Different colors for alternating tiles
2. **Texture Mapping**: Optional texture sampling
3. **Bounded Plane**: Intersection only within specified rectangular area

```cpp
Color Floor::getSurfaceColor(Vector3D point) {
    if (useTexture && textureImage != nullptr) {
        // Calculate tile position
        int tileX = (int)((point.x - referencePoint.x) / tileSize);
        int tileY = (int)((point.y - referencePoint.y) / tileSize);
        
        // Calculate local coordinates within tile
        double localX = (point.x - referencePoint.x) - tileX * tileSize;
        double localY = (point.y - referencePoint.y) - tileY * tileSize;
        
        // Convert to texture coordinates [0,1]
        double u = localX / tileSize;
        double v = localY / tileSize;
        
        return sampleTexture(u, v);
    } else {
        // Default checkerboard pattern
        return getCheckerboardColor(point);
    }
}
```

## 5. General Object Architecture

### 5.1 Polymorphic Design
Your ray tracer uses object-oriented design with virtual functions:

```cpp
class Object {
public:
    virtual double intersect(Ray *ray) = 0;           // Pure virtual
    virtual Vector3D computeNormal(Vector3D point) = 0;  // Pure virtual
    virtual Color getSurfaceColor(Vector3D point);    // Virtual with default
    virtual void draw();                              // Virtual with default
};
```

### 5.2 Intersection Pipeline
Each object type implements the intersection method differently:

```cpp
// Sphere: Quadratic equation solution
double Sphere::intersect(Ray *ray) {
    // Solve: |ray(t) - center|² = radius²
    // Returns closest positive t or -1
}

// Triangle: Plane intersection + inside test
double Triangle::intersect(Ray *ray) {
    // 1. Intersect with triangle's plane
    // 2. Test if point is inside triangle
    // Returns t or -1
}

// Floor: Bounded plane intersection
double Floor::intersect(Ray *ray) {
    // 1. Intersect with horizontal plane
    // 2. Test if point is within floor bounds
    // Returns t or -1
}
```

### 5.3 Lighting Integration
Your `Object` class includes comprehensive lighting calculations:

```cpp
void Object::phongLighting(Ray *ray, Color *color, int level) {
    double tmin = intersect(ray);
    if (tmin < 0) return;
    
    Vector3D intersectionPoint = ray->getOrigin() + ray->getDirection() * tmin;
    Color surfaceColor = getSurfaceColor(intersectionPoint);
    
    // Ambient component
    *color = surfaceColor * materialCoefficients.getAmbient();
    
    // Point light contributions
    computePointLightContribution(intersectionPoint, ray, color, surfaceColor);
    
    // Spot light contributions  
    computeSpotLightContribution(intersectionPoint, ray, color, surfaceColor);
    
    // Recursive reflections
    if (level < maxRecursionLevel) {
        computeReflection(intersectionPoint, ray, color, level);
    }
}
```

### 5.4 Shadow Testing
Your implementation includes sophisticated shadow testing:

```cpp
bool Object::isInShadow(Vector3D intersectionPoint, Vector3D lightPosition, double lightDistance) {
    Ray shadowRay(intersectionPoint + normal * epsilon, lightPosition - intersectionPoint);
    
    for (Object *obj : objects) {
        if (obj != this) {  // Don't shadow test against self
            double t = obj->intersect(&shadowRay);
            if (t > 0 && t < lightDistance) {
                return true;  // Another object blocks the light
            }
        }
    }
    return false;
}
```

Key Implementation Features:
1. **Object-Oriented Design**: Clean separation of concerns
2. **Polymorphic Intersection**: Each object type handles its own geometry
3. **Comprehensive Lighting**: Ambient, diffuse, specular, and reflection
4. **Shadow Testing**: Accurate shadow computation for all light types
5. **Texture Support**: Advanced surface coloring with texture mapping
6. **Efficient Algorithms**: Optimized intersection tests for each geometry type

The architecture demonstrates excellent software engineering practices while maintaining mathematical accuracy for realistic ray tracing results.
