# Phong Illumination Calculations: Step-by-Step Example

## Overview

This document demonstrates the complete Phong illumination calculation process using concrete numerical examples. We'll walk through calculating the final color of a pixel when a camera ray hits a surface, showing all the mathematical steps involved in ambient, diffuse, and specular lighting calculations.

---

## Scene Setup

### Camera Configuration
```
Camera Position: (0, 0, 10)
Camera Look Direction: (0, 0, -1)  // Looking down negative Z-axis
```

### Object: Red Sphere
```
Sphere Center: (2, 1, 0)
Sphere Radius: 1.5
Surface Color: (0.8, 0.2, 0.2)  // Red surface
Material Coefficients:
  - Ambient: 0.2
  - Diffuse: 0.6
  - Specular: 0.4
  - Reflection: 0.1
Shininess: 50
```

### Light Sources

**Point Light:**
```
Position: (5, 5, 5)
Color: (1.0, 1.0, 1.0)  // White light
```

**Spot Light:**
```
Position: (-3, 4, 3)
Color: (0.8, 0.8, 1.0)  // Slightly blue-white light
Direction: (1, -1, -1)  // Normalized: (0.577, -0.577, -0.577)
Cutoff Angle: 30°
```

---

## Step 1: Ray-Surface Intersection Result

**Given:** A camera ray hits the sphere at intersection point `P = (2.8, 1.6, 0.4)`

### Surface Properties at Intersection
```
Surface Normal: N = (P - Center) / Radius
N = ((2.8, 1.6, 0.4) - (2, 1, 0)) / 1.5
N = (0.8, 0.6, 0.4) / 1.5
N = (0.533, 0.4, 0.267)  // Normalized surface normal

Surface Color: C_surface = (0.8, 0.2, 0.2)
```

### Viewing Direction
```
V = Normalized(Camera - Intersection)
V = Normalized((0, 0, 10) - (2.8, 1.6, 0.4))
V = Normalized((-2.8, -1.6, 9.6))
V = (-0.277, -0.158, 0.949)  // Normalized viewing direction
```

---

## Step 2: Ambient Lighting Calculation

### Formula
```
I_ambient = k_a × C_surface
```

### Calculation
```
k_a = 0.2 (ambient coefficient)
C_surface = (0.8, 0.2, 0.2)

I_ambient = 0.2 × (0.8, 0.2, 0.2)
I_ambient = (0.16, 0.04, 0.04)
```

**Result:** Base ambient color = **(0.16, 0.04, 0.04)**

---

## Step 3: Point Light Contribution

### 3.1 Light Direction and Distance
```
L_pos = (5, 5, 5)
P = (2.8, 1.6, 0.4)

Light Vector = L_pos - P = (5, 5, 5) - (2.8, 1.6, 0.4) = (2.2, 3.4, 4.6)
Distance = |Light Vector| = √(2.2² + 3.4² + 4.6²) = √(4.84 + 11.56 + 21.16) = √37.56 = 6.13

L = Normalized Light Direction = (2.2, 3.4, 4.6) / 6.13
L = (0.359, 0.555, 0.751)
```

### 3.2 Shadow Testing
**Assumption:** No objects block the light path, so the surface is illuminated.

### 3.3 Diffuse Component Calculation

#### Lambert's Cosine Law
```
N·L = (0.533, 0.4, 0.267) · (0.359, 0.555, 0.751)
N·L = (0.533 × 0.359) + (0.4 × 0.555) + (0.267 × 0.751)
N·L = 0.191 + 0.222 + 0.201 = 0.614

diffuse_factor = max(0, N·L) = max(0, 0.614) = 0.614
```

#### Diffuse Color Contribution
```
I_diffuse = k_d × I_light × C_surface × diffuse_factor
k_d = 0.6 (diffuse coefficient)
I_light = (1.0, 1.0, 1.0) (white point light)
C_surface = (0.8, 0.2, 0.2)
diffuse_factor = 0.614

I_diffuse = 0.6 × (1.0, 1.0, 1.0) × (0.8, 0.2, 0.2) × 0.614

For each channel:
Red:   0.6 × 1.0 × 0.8 × 0.614 = 0.295
Green: 0.6 × 1.0 × 0.2 × 0.614 = 0.074
Blue:  0.6 × 1.0 × 0.2 × 0.614 = 0.074

I_diffuse = (0.295, 0.074, 0.074)
```

### 3.4 Specular Component Calculation

#### Reflection Direction
```
R = I - 2(I·N)N
I = -L = (-0.359, -0.555, -0.751)  // Incident direction (toward surface)
N = (0.533, 0.4, 0.267)

I·N = (-0.359 × 0.533) + (-0.555 × 0.4) + (-0.751 × 0.267)
I·N = -0.191 - 0.222 - 0.201 = -0.614

R = (-0.359, -0.555, -0.751) - 2(-0.614)(0.533, 0.4, 0.267)
R = (-0.359, -0.555, -0.751) + 1.228(0.533, 0.4, 0.267)
R = (-0.359, -0.555, -0.751) + (0.655, 0.491, 0.328)
R = (0.296, -0.064, -0.423)
```

#### Specular Factor
```
R·V = (0.296, -0.064, -0.423) · (-0.277, -0.158, 0.949)
R·V = (0.296 × -0.277) + (-0.064 × -0.158) + (-0.423 × 0.949)
R·V = -0.082 + 0.010 - 0.401 = -0.473

specular_factor = max(0, R·V) = max(0, -0.473) = 0.0
```

#### Specular Color Contribution
```
Since specular_factor = 0, there's no specular highlight from this point light.
I_specular = (0.0, 0.0, 0.0)
```

**Point Light Total Contribution:**
```
Point_Light_Contribution = I_diffuse + I_specular
Point_Light_Contribution = (0.295, 0.074, 0.074) + (0.0, 0.0, 0.0)
Point_Light_Contribution = (0.295, 0.074, 0.074)
```

---

## Step 4: Spot Light Contribution

### 4.1 Light Direction and Distance
```
L_pos = (-3, 4, 3)
P = (2.8, 1.6, 0.4)

Light Vector = L_pos - P = (-3, 4, 3) - (2.8, 1.6, 0.4) = (-5.8, 2.4, 2.6)
Distance = |Light Vector| = √((-5.8)² + 2.4² + 2.6²) = √(33.64 + 5.76 + 6.76) = √46.16 = 6.79

L = Normalized Light Direction = (-5.8, 2.4, 2.6) / 6.79
L = (-0.854, 0.353, 0.383)
```

### 4.2 Spot Light Cone Check
```
Spot_Direction = (0.577, -0.577, -0.577)  // Normalized
Incident_Direction = L = (-0.854, 0.353, 0.383)

cos(β) = L · Spot_Direction
cos(β) = (-0.854 × 0.577) + (0.353 × -0.577) + (0.383 × -0.577)
cos(β) = -0.493 - 0.204 - 0.221 = -0.918

β = arccos(-0.918) = 156.7°

Since β = 156.7° > 30° (cutoff angle), the surface is outside the spot light cone.
```

**Spot Light Contribution:** **(0.0, 0.0, 0.0)** - No illumination

---

## Step 5: Reflection Calculation

### 5.1 Reflection Direction
```
Observer Ray Direction = -V = (0.277, 0.158, -0.949)
Surface Normal = N = (0.533, 0.4, 0.267)

Reflection Direction = Observer_Ray - 2(Observer_Ray·N)N
Observer_Ray·N = (0.277 × 0.533) + (0.158 × 0.4) + (-0.949 × 0.267)
Observer_Ray·N = 0.148 + 0.063 - 0.253 = -0.042

Reflection_Direction = (0.277, 0.158, -0.949) - 2(-0.042)(0.533, 0.4, 0.267)
Reflection_Direction = (0.277, 0.158, -0.949) + 0.084(0.533, 0.4, 0.267)
Reflection_Direction = (0.277, 0.158, -0.949) + (0.045, 0.034, 0.022)
Reflection_Direction = (0.322, 0.192, -0.927)
```

### 5.2 Reflection Ray Intersection
**Assumption:** The reflection ray hits another object with calculated color **(0.3, 0.6, 0.4)**

### 5.3 Reflection Contribution
```
k_r = 0.1 (reflection coefficient)
Reflected_Color = (0.3, 0.6, 0.4)

I_reflection = k_r × Reflected_Color
I_reflection = 0.1 × (0.3, 0.6, 0.4)
I_reflection = (0.03, 0.06, 0.04)
```

---

## Step 6: Final Color Calculation

### 6.1 Color Accumulation
```
Final_Color = I_ambient + Point_Light_Contribution + Spot_Light_Contribution + I_reflection

Final_Color = (0.16, 0.04, 0.04) + (0.295, 0.074, 0.074) + (0.0, 0.0, 0.0) + (0.03, 0.06, 0.04)

Final_Color = (0.16 + 0.295 + 0.0 + 0.03, 0.04 + 0.074 + 0.0 + 0.06, 0.04 + 0.074 + 0.0 + 0.04)

Final_Color = (0.485, 0.174, 0.154)
```

### 6.2 Color Clamping
```
Final_Color = (min(1.0, 0.485), min(1.0, 0.174), min(1.0, 0.154))
Final_Color = (max(0.0, 0.485), max(0.0, 0.174), max(0.0, 0.154))
Final_Color = (0.485, 0.174, 0.154)
```

---

## Step 7: Alternative Scenario with Specular Highlight

### Modified Camera Position for Specular Effect
Let's recalculate with a camera position that would see the specular highlight:

**Camera Position:** (6, 6, 6)
**Viewing Direction:** V = Normalized((6, 6, 6) - (2.8, 1.6, 0.4)) = (0.553, 0.761, 0.969)

### Recalculated Specular Component
```
R = (0.296, -0.064, -0.423)  // Same as before
V = (0.553, 0.761, 0.969)     // New viewing direction

R·V = (0.296 × 0.553) + (-0.064 × 0.761) + (-0.423 × 0.969)
R·V = 0.164 - 0.049 - 0.410 = -0.295

specular_factor = max(0, R·V) = max(0, -0.295) = 0.0
```

Still no specular highlight. Let's try a position that would create one:

**Camera Position:** (3, 5, 6)
**Viewing Direction:** V = Normalized((3, 5, 6) - (2.8, 1.6, 0.4)) = (0.035, 0.581, 0.813)

### Recalculated Specular Component
```
R·V = (0.296 × 0.035) + (-0.064 × 0.581) + (-0.423 × 0.813)
R·V = 0.010 - 0.037 - 0.344 = -0.371

specular_factor = max(0, R·V) = 0.0
```

### Creating a Specular Highlight Scenario
Let's modify the light position to create a specular highlight:

**Point Light Position:** (1, 2, 8)
**Intersection Point:** P = (2.8, 1.6, 0.4)
**Camera Position:** (0, 0, 10)

### Recalculated Light Direction
```
Light Vector = (1, 2, 8) - (2.8, 1.6, 0.4) = (-1.8, 0.4, 7.6)
Distance = √((-1.8)² + 0.4² + 7.6²) = √(3.24 + 0.16 + 57.76) = √61.16 = 7.82

L = (-1.8, 0.4, 7.6) / 7.82 = (-0.230, 0.051, 0.972)
```

### Recalculated Reflection Direction
```
I = -L = (0.230, -0.051, -0.972)
N = (0.533, 0.4, 0.267)

I·N = (0.230 × 0.533) + (-0.051 × 0.4) + (-0.972 × 0.267)
I·N = 0.123 - 0.020 - 0.259 = -0.156

R = (0.230, -0.051, -0.972) - 2(-0.156)(0.533, 0.4, 0.267)
R = (0.230, -0.051, -0.972) + 0.312(0.533, 0.4, 0.267)
R = (0.230, -0.051, -0.972) + (0.166, 0.125, 0.083)
R = (0.396, 0.074, -0.889)
```

### Specular Calculation
```
V = (-0.277, -0.158, 0.949)
R = (0.396, 0.074, -0.889)

R·V = (0.396 × -0.277) + (0.074 × -0.158) + (-0.889 × 0.949)
R·V = -0.110 - 0.012 - 0.844 = -0.966

specular_factor = max(0, R·V) = 0.0
```

### Final Specular Highlight Example
Let's create an ideal scenario where the viewing direction aligns with the reflection:

**Reflection Direction:** R = (0.2, 0.3, 0.9)
**Viewing Direction:** V = (0.2, 0.3, 0.9)  // Perfectly aligned

```
R·V = (0.2 × 0.2) + (0.3 × 0.3) + (0.9 × 0.9) = 0.04 + 0.09 + 0.81 = 0.94

specular_factor = max(0, R·V) = 0.94
specular_with_shininess = (0.94)^50 = 0.0067

I_specular = k_s × I_light × specular_with_shininess
I_specular = 0.4 × (1.0, 1.0, 1.0) × 0.0067
I_specular = (0.0027, 0.0027, 0.0027)
```

---

## Step 8: Summary of All Calculations

### Component Breakdown
```
Ambient Component:     (0.16, 0.04, 0.04)
Point Light Diffuse:   (0.295, 0.074, 0.074)
Point Light Specular:  (0.0, 0.0, 0.0)
Spot Light:            (0.0, 0.0, 0.0) - outside cone
Reflection:            (0.03, 0.06, 0.04)

Final Color:           (0.485, 0.174, 0.154)
```

### Key Insights from Calculations

1. **Ambient Lighting:** Provides base illumination proportional to surface color
2. **Diffuse Lighting:** Depends on angle between light and surface normal (Lambert's law)
3. **Specular Lighting:** Requires alignment between reflection and viewing directions
4. **Spot Light:** Only illuminates surfaces within the cone angle
5. **Reflection:** Adds environmental reflections weighted by reflection coefficient

### Mathematical Formulas Used

**Dot Product:**
```
A·B = A_x×B_x + A_y×B_y + A_z×B_z
```

**Vector Length:**
```
|V| = √(V_x² + V_y² + V_z²)
```

**Normalization:**
```
V_normalized = V / |V|
```

**Reflection:**
```
R = I - 2(I·N)N
```

**Phong Components:**
```
I_ambient = k_a × C_surface
I_diffuse = k_d × I_light × C_surface × max(0, N·L)
I_specular = k_s × I_light × (R·V)^n
```

This detailed calculation shows exactly how the Phong illumination model transforms geometric relationships between lights, surfaces, and viewers into realistic lighting effects through mathematical formulas.
