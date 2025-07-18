# Point Light Implementation in Ray Tracing

## Overview
Point lights are omnidirectional light sources that emit light uniformly in all directions from a single point in 3D space. In this ray tracing implementation, point lights are fundamental for creating realistic lighting effects including diffuse shading, specular highlights, and shadows.

## Class Structure

### Header File: `2005107_PointLight.h`

```cpp
#pragma once

#include <iostream>
using namespace std;

#include "../Vector3D/2005107_Vector3D.h"
#include "../Color/2005107_Color.h"

class PointLight
{
    Vector3D lightPosition;  // 3D position of the light source
    Color color;            // RGB color of the light

public:
    PointLight();
    PointLight setLightPosition(double x, double y, double z);
    PointLight setColor(Color color);
    Vector3D getLightPosition() const;
    Color getColor() const;
    void draw();
    friend ostream &operator<<(ostream &out, const PointLight &p);
};
```

#### Private Members:
- **`Vector3D lightPosition`**: Stores the 3D coordinates (x, y, z) where the light source is located
- **`Color color`**: Stores the RGB color values of the light (affects the color of illuminated objects)

#### Public Methods:
- **Constructor**: Initializes the light at origin with black color
- **Setters**: Configure light position and color
- **Getters**: Access light properties
- **Draw**: Renders the light as a visual sphere in OpenGL
- **Stream operator**: Outputs light information for debugging

---

## Implementation Details

### Constructor
```cpp
PointLight::PointLight() : lightPosition(Vector3D::zero())
{
    color = Color(0, 0, 0);
}
```
**Line-by-line explanation:**
- **Line 1**: Constructor initialization list sets `lightPosition` to zero vector (0, 0, 0)
- **Line 3**: Sets the light color to black (0, 0, 0) - essentially no light emission

### Position Setter
```cpp
PointLight PointLight::setLightPosition(double x, double y, double z)
{
    lightPosition.x = x;
    lightPosition.y = y;
    lightPosition.z = z;
    return *this;
}
```
**Line-by-line explanation:**
- **Line 1**: Method signature accepting 3D coordinates
- **Line 3-5**: Directly assigns x, y, z coordinates to the lightPosition vector components
- **Line 6**: Returns reference to self for method chaining (fluent interface)

### Color Setter
```cpp
PointLight PointLight::setColor(Color color)
{
    this->color = color;
    return *this;
}
```
**Line-by-line explanation:**
- **Line 1**: Accepts a Color object parameter
- **Line 3**: Uses `this->` to distinguish between parameter and member variable
- **Line 4**: Returns self-reference for method chaining

### Position Getter
```cpp
Vector3D PointLight::getLightPosition() const
{
    return lightPosition;
}
```
**Line-by-line explanation:**
- **Line 1**: Const method (doesn't modify object state)
- **Line 3**: Returns copy of the light position vector

### Color Getter
```cpp
Color PointLight::getColor() const
{
    return color;
}
```
**Line-by-line explanation:**
- **Line 1**: Const method ensuring no modification
- **Line 3**: Returns copy of the light color

### OpenGL Visualization
```cpp
void PointLight::draw()
{
    glPushMatrix();
    {
        glColor3f(color.getRed(), color.getGreen(), color.getBlue());
        glTranslatef(lightPosition.x, lightPosition.y, lightPosition.z);
        glutSolidSphere(5, 200, 200);
    }
    glPopMatrix();
}
```
**Line-by-line explanation:**
- **Line 3**: Saves current OpenGL transformation matrix
- **Line 5**: Sets OpenGL color to match light color (RGB components)
- **Line 6**: Translates to light position in 3D space
- **Line 7**: Draws a solid sphere (radius=5, 200 longitude/latitude segments for smoothness)
- **Line 9**: Restores previous transformation matrix

### Debug Output
```cpp
ostream &operator<<(ostream &out, const PointLight &p)
{
    out << "Light Position: " << p.lightPosition << " Color: " 
        << p.color.getRed() << " " << p.color.getGreen() << " " << p.color.getBlue();
    return out;
}
```
**Line-by-line explanation:**
- **Line 1**: Overloaded stream insertion operator for debugging/logging
- **Line 3-4**: Outputs formatted string with position and RGB color values
- **Line 5**: Returns output stream for chaining

---

## Lighting Calculations

### Point Light Contribution Function
```cpp
void Object::computePointLightContribution(Vector3D intersectionPoint, Ray *observerRay, Color *color, Color intersectionPointColor)
{
    Ray normalRay = Ray(intersectionPoint, computeNormal(intersectionPoint));
    
    for (PointLight *pointLight : pointLights)
    {
        Ray incidentRay = Ray(pointLight->getLightPosition(), intersectionPoint - pointLight->getLightPosition());
        Vector3D reflectedDirection = getReflectionDirection(incidentRay.getDirection(), normalRay.getDirection());
        Ray reflectedRay = Ray(intersectionPoint, reflectedDirection);

        double distance = (pointLight->getLightPosition() - intersectionPoint).length();
        if (distance < epsilon)
        {
            continue;
        }

        if (!isInShadow(intersectionPoint, pointLight->getLightPosition(), distance))
        {
            double diffuse = computeDiffuseComponent(incidentRay.getDirection(), normalRay.getDirection());
            double specular = computeSpecularComponent(reflectedRay.getDirection(), observerRay->getDirection(), shine);

            // diffuse reflection
            color->setRed(color->getRed() + pointLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getDiffuse() * diffuse));
            color->setGreen(color->getGreen() + pointLight->getColor().getGreen() * intersectionPointColor.getGreen() * (materialCoefficients.getDiffuse() * diffuse));
            color->setBlue(color->getBlue() + pointLight->getColor().getBlue() * intersectionPointColor.getBlue() * (materialCoefficients.getDiffuse() * diffuse));

            // specular reflection
            color->setRed(color->getRed() + pointLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getSpecular() * pow(specular, shine)));
            color->setGreen(color->getGreen() + pointLight->getColor().getGreen() * intersectionPointColor.getGreen() * (materialCoefficients.getSpecular() * pow(specular, shine)));
            color->setBlue(color->getBlue() + pointLight->getColor().getBlue() * intersectionPointColor.getBlue() * (materialCoefficients.getSpecular() * pow(specular, shine)));
        }
    }
}
```

**Detailed Line-by-line Analysis:**

**Line 3**: `Ray normalRay = Ray(intersectionPoint, computeNormal(intersectionPoint));`
- Creates a ray representing the surface normal at the intersection point
- Used for calculating light reflection and surface orientation

**Line 5**: `for (PointLight *pointLight : pointLights)`
- Iterates through all point lights in the scene
- Each light contributes to the final color independently

**Line 7**: `Ray incidentRay = Ray(pointLight->getLightPosition(), intersectionPoint - pointLight->getLightPosition());`
- Creates incident ray from light source to intersection point
- Direction vector: `intersectionPoint - lightPosition` (pointing toward surface)

**Line 8**: `Vector3D reflectedDirection = getReflectionDirection(incidentRay.getDirection(), normalRay.getDirection());`
- Calculates perfect reflection direction using incident ray and surface normal
- Uses formula: `R = I - 2(I·N)N` where I=incident, N=normal, R=reflected

**Line 9**: `Ray reflectedRay = Ray(intersectionPoint, reflectedDirection);`
- Creates ray representing the reflected light direction
- Used for specular highlight calculations

**Line 11**: `double distance = (pointLight->getLightPosition() - intersectionPoint).length();`
- Calculates Euclidean distance between light and intersection point
- Used for shadow testing and potential light attenuation

**Line 12-15**: Epsilon check
- Prevents division by zero or numerical instability
- Skips lights that are extremely close to the surface

**Line 17**: `if (!isInShadow(intersectionPoint, pointLight->getLightPosition(), distance))`
- Shadow testing: checks if point is visible from light source
- Only applies lighting if the point is not in shadow

**Line 19**: `double diffuse = computeDiffuseComponent(incidentRay.getDirection(), normalRay.getDirection());`
- Calculates Lambertian diffuse reflection coefficient
- Formula: `max(0, -L·N)` where L=light direction, N=surface normal

**Line 20**: `double specular = computeSpecularComponent(reflectedRay.getDirection(), observerRay->getDirection(), shine);`
- Calculates specular reflection coefficient for highlights
- Formula: `max(0, -R·V)` where R=reflected direction, V=view direction

**Lines 22-24**: Diffuse Color Contribution
```cpp
color->setRed(color->getRed() + pointLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getDiffuse() * diffuse));
```
- Adds diffuse lighting contribution to final color
- Formula: `finalColor += lightColor * surfaceColor * diffuseCoeff * lambertTerm`
- Applied separately to R, G, B components

**Lines 26-28**: Specular Color Contribution
```cpp
color->setRed(color->getRed() + pointLight->getColor().getRed() * intersectionPointColor.getRed() * (materialCoefficients.getSpecular() * pow(specular, shine)));
```
- Adds specular highlights to final color
- Formula: `finalColor += lightColor * surfaceColor * specularCoeff * (R·V)^shininess`
- `pow(specular, shine)` creates focused highlights

---

## Helper Functions

### Shadow Testing
```cpp
bool Object::isInShadow(Vector3D intersectionPoint, Vector3D lightPosition, double lightDistance)
{
    Ray shadowRay = Ray(lightPosition, intersectionPoint - lightPosition);
    
    for (Object *object : objects)
    {
        double t = object->intersect(&shadowRay);
        if (t > 0 && t + epsilon < lightDistance)
        {
            return true;
        }
    }
    return false;
}
```
**Line-by-line explanation:**
- **Line 3**: Creates shadow ray from light to intersection point
- **Line 5**: Tests intersection with all objects in scene
- **Line 7**: Gets intersection distance along shadow ray
- **Line 8**: Point is in shadow if another object is closer than the light
- **Line 10**: Returns true if any object blocks the light

### Diffuse Component Calculation
```cpp
double Object::computeDiffuseComponent(Vector3D incidentDirection, Vector3D normalDirection)
{
    return std::max(-(incidentDirection * normalDirection), 0.0);
}
```
**Line-by-line explanation:**
- **Line 3**: Implements Lambert's cosine law: `max(0, -L·N)`
- Negative sign because incident direction points toward surface
- Clamps to 0 for angles > 90° (no negative lighting)

### Specular Component Calculation
```cpp
double Object::computeSpecularComponent(Vector3D reflectedDirection, Vector3D observerDirection, int shininess)
{
    return std::max(-(reflectedDirection * observerDirection), 0.0);
}
```
**Line-by-line explanation:**
- **Line 3**: Calculates `max(0, -R·V)` for specular highlights
- Negative sign accounts for direction conventions
- Result is raised to `shininess` power in calling function

### Reflection Direction Calculation
```cpp
Vector3D Object::getReflectionDirection(Vector3D incidentDirection, Vector3D normalDirection)
{
    return incidentDirection - normalDirection * (2 * (incidentDirection * normalDirection));
}
```
**Line-by-line explanation:**
- **Line 3**: Implements reflection formula: `R = I - 2(I·N)N`
- `I`: incident direction
- `N`: surface normal
- `I·N`: dot product (scalar)
- `2(I·N)N`: projection of incident onto normal, scaled by 2

---

## Input File Format

### Point Light Data Loading
```cpp
void loadPointLights(ifstream &in)
{
    int numberOfPointLights;
    in >> numberOfPointLights;
    
    for (int i = 0; i < numberOfPointLights; i++)
    {
        double x, y, z;
        in >> x >> y >> z;
        
        double r, g, b;
        in >> r >> g >> b;
        
        PointLight *pointLight = new PointLight();
        pointLight->setLightPosition(x, y, z);
        pointLight->setColor(Color(r, g, b));
        
        pointLights.push_back(pointLight);
    }
}
```

### Input Format Example
```
4                    // Number of point lights
70.0 70.0 70.0      // Light 1: Position (x, y, z)
1.0 0.0 0.0         // Light 1: Color (r, g, b) - Red light
-70 70 70           // Light 2: Position
0.0 0.0 1.0         // Light 2: Color - Blue light
70 -70 70           // Light 3: Position
1 0 0.0             // Light 3: Color - Red light
-70 -70 70          // Light 4: Position
0 1.0 0             // Light 4: Color - Green light
```

---

## Physical Properties

### Light Attenuation
Currently, the implementation doesn't include distance-based attenuation, but it could be added:
```cpp
double attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);
```

### Light Intensity
The intensity is controlled by the RGB color values:
- **White light (1, 1, 1)**: Maximum intensity
- **Colored light (1, 0, 0)**: Red light with full intensity
- **Dim light (0.5, 0.5, 0.5)**: Half intensity white light

---

## Integration with Ray Tracing

### Usage in Ray Tracing Pipeline
1. **Ray-Object Intersection**: Find where camera ray hits objects
2. **Surface Properties**: Get surface normal and material properties
3. **Light Calculation**: For each point light, calculate lighting contribution
4. **Shadow Testing**: Check if light is blocked by other objects
5. **Color Accumulation**: Sum contributions from all lights

### Phong Lighting Model Components
- **Ambient**: `ambient_coeff * object_color`
- **Diffuse**: `diffuse_coeff * object_color * light_color * max(0, N·L)`
- **Specular**: `specular_coeff * light_color * (R·V)^shininess`

### Performance Considerations
- **Shadow rays**: Each point light requires shadow testing
- **Multiple lights**: Computational cost scales with number of lights
- **Optimization**: Early termination, spatial data structures, light culling

---

---

## Mathematical Foundation of Point Light Calculations

### The Complete Story: From Photons to Pixels

The mathematical journey of point light calculation in ray tracing follows the path of light from its source to the viewer's eye, simulating the physics of light interaction with matter.

### Step 1: The Light Source Model

A point light is mathematically modeled as an infinitesimal point in 3D space that emits light uniformly in all directions. The light source has:
- **Position**: P_light = (x_l, y_l, z_l)
- **Color/Intensity**: I_light = (R_l, G_l, B_l)

The light follows the **inverse square law** in real physics, where intensity decreases with distance squared:
```
I_received = I_source / (4π × distance²)
```

However, in computer graphics, we often omit this for artistic control, or use modified attenuation functions.

### Step 2: Ray-Surface Intersection

When a camera ray hits a surface at point P_surface, we need to determine how much light from our point light reaches this surface point. The key vectors are:

- **Surface Point**: P_surface = (x_s, y_s, z_s)
- **Light Direction**: L = normalized(P_light - P_surface)
- **Surface Normal**: N = surface_normal_at(P_surface)
- **View Direction**: V = normalized(P_camera - P_surface)

### Step 3: The Phong Illumination Model

The Phong model breaks lighting into three components, each with its own mathematical foundation:

#### 3.1 Ambient Reflection
Ambient light simulates indirect illumination from the environment:
```
I_ambient = k_a × I_light × C_surface
```
Where:
- k_a = ambient reflection coefficient (0 ≤ k_a ≤ 1)
- I_light = light intensity
- C_surface = surface color

**Physical Meaning**: This represents the base level of illumination when no direct light hits the surface, simulating light bouncing from other surfaces in the environment.

#### 3.2 Diffuse Reflection (Lambertian Shading)

Diffuse reflection follows Lambert's Cosine Law, based on the angle between the light direction and surface normal:
```
I_diffuse = k_d × I_light × C_surface × max(0, N · L)
```

**The Mathematics Behind Lambert's Law**:
- The dot product N · L gives cos(θ), where θ is the angle between normal and light
- cos(θ) = 1 when light hits perpendicular to surface (maximum brightness)
- cos(θ) = 0 when light hits parallel to surface (no illumination)
- cos(θ) < 0 when light comes from behind surface (clamped to 0)

**Physical Meaning**: This simulates how rough surfaces scatter light uniformly in all directions. The intensity depends on how much surface area is "presented" to the light source.

#### 3.3 Specular Reflection (Phong Highlights)

Specular reflection creates shiny highlights based on the reflection of light toward the viewer:
```
I_specular = k_s × I_light × (R · V)^n
```

Where:
- R = reflection direction = 2(N · L)N - L
- V = view direction (toward camera)
- n = shininess exponent (higher = more focused highlight)

**The Reflection Vector Mathematics**:
The reflection vector R is computed using the law of reflection:
```
R = I - 2(I · N)N
```
Where I is the incident light direction.

**Physical Meaning**: This simulates mirror-like reflection. The specular term is highest when the reflection direction aligns with the viewing direction.

### Step 4: The Complete Lighting Equation

The final pixel color combines all three components:
```
I_final = I_ambient + I_diffuse + I_specular
```

Expanding this:
```
I_final = k_a × I_light × C_surface + 
          k_d × I_light × C_surface × max(0, N · L) + 
          k_s × I_light × (R · V)^n
```

For RGB color channels, this calculation is performed independently for each channel.

### Step 5: Shadow Calculation

Before applying lighting, we must determine if the surface point is in shadow:

**Shadow Ray Test**:
1. Cast a ray from P_surface toward P_light
2. Check if any object intersects this ray before reaching the light
3. If intersection exists at distance t, and t < distance_to_light, then the point is in shadow

**Mathematical Test**:
```
Shadow_ray = Ray(P_surface + ε×N, P_light - P_surface)
```
The epsilon offset prevents self-shadowing due to floating-point precision errors.

### Step 6: Multiple Light Sources

When multiple point lights exist, the final illumination is the sum of contributions from all lights:
```
I_total = I_ambient + Σ(I_diffuse_i + I_specular_i) for all lights i
```

Each light contributes independently to the diffuse and specular terms, but ambient is typically computed once.

### Step 7: Vector Mathematics in Detail

#### Dot Product Significance:
The dot product between two normalized vectors gives the cosine of the angle between them:
```
A · B = |A| × |B| × cos(θ) = cos(θ) (for normalized vectors)
```

This is fundamental to lighting calculations:
- N · L = cos(angle between normal and light)
- R · V = cos(angle between reflection and view)

#### Normal Vector Calculation:
For different surface types:
- **Sphere**: N = (P_surface - P_center) / radius
- **Plane**: N = plane_normal (constant)
- **Triangle**: N = (edge1 × edge2) normalized

### Step 8: Energy Conservation

In physically-based rendering, the coefficients must satisfy energy conservation:
```
k_a + k_d + k_s ≤ 1
```

This ensures that the surface doesn't reflect more light than it receives.

### Step 9: Color Channel Processing

Each RGB channel is processed independently:
```
Red_final = k_a × Red_light × Red_surface + 
            k_d × Red_light × Red_surface × max(0, N · L) + 
            k_s × Red_light × (R · V)^n

Green_final = k_a × Green_light × Green_surface + 
              k_d × Green_light × Green_surface × max(0, N · L) + 
              k_s × Green_light × (R · V)^n

Blue_final = k_a × Blue_light × Blue_surface + 
             k_d × Blue_light × Blue_surface × max(0, N · L) + 
             k_s × Blue_light × (R · V)^n
```

### Step 10: Numerical Considerations

#### Floating-Point Precision:
- Use epsilon values to prevent self-intersection
- Clamp negative values to zero
- Normalize vectors to prevent magnitude errors

#### Performance Optimizations:
- Early termination if N · L ≤ 0 (surface faces away from light)
- Skip shadow testing for lights behind surfaces
- Use fast inverse square root for normalization

### The Physics Behind the Mathematics

#### Why the Dot Product Works:
The dot product naturally captures the relationship between surface orientation and light direction. When a surface faces directly toward a light (θ = 0°), cos(0°) = 1, giving maximum illumination. When the surface is perpendicular to the light (θ = 90°), cos(90°) = 0, giving no illumination.

#### Why the Power Function for Specular:
The power function (R · V)^n models how light scatters from a surface. Higher powers create more focused highlights, simulating smoother surfaces. This approximates the Fresnel reflection phenomenon and microfacet distribution.

#### Why Linear Color Addition:
Light is additive in nature. When multiple light sources illuminate a surface, their contributions simply add together, following the principle of superposition in electromagnetic theory.

### Real-World Applications

This mathematical model, while simplified, captures the essential physics of light interaction:
- **Diffuse reflection** dominates on matte surfaces like paper or unpolished wood
- **Specular reflection** dominates on shiny surfaces like metal or water
- **Ambient lighting** simulates the complex interreflection of light in real environments
- **Shadow casting** adds depth and realism by simulating light occlusion

The beauty of this model is that it provides physically plausible results while being computationally efficient enough for real-time rendering and ray tracing applications.

---

## Summary

Point lights in this ray tracing implementation provide realistic omnidirectional lighting with:
- **Position-based lighting**: Light intensity depends on surface orientation
- **Color mixing**: Light color blends with surface color
- **Shadow casting**: Objects block light creating realistic shadows
- **Specular highlights**: Shiny surfaces reflect light toward viewer
- **Phong shading**: Industry-standard lighting model for realism

The implementation follows physically-based principles while remaining computationally efficient for real-time preview and high-quality offline rendering.
