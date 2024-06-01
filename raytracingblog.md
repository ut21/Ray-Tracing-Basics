# Balls and More: Experiments With Ray Tracers


## Yapping and background
Last semester I worked with Neural Radiance Fields for my project with Prof. Sundaresan Raman (as it happens with most SOPs at BITS, the project didn't really go anywhere and I should be getting an A regardless :) )

I treated the graphics rendering part of the NeRF pipeline as a blackbox and the neural architecture itself as just a function approximator for an abstract plenoptic function (link).

Until some time ago when I came across Peter Shirley's (link) Ray Tracing Weekend Series and made a rudimentary ray tracer of my own.

## Viewing the image and the PPM format

The first thing we will do is to set up how we are going to view the images. To do this, we will write values to a file in the PPM (Portable Pixmap) format. To have a taster write the following in a .ppm file.

```
P3
3 2
255
255 0 0 0 255 0 0 0 255
255 255 0 255 255 255 0 0 0
```

To view a PPM file I am using an extension within VS Code. The output looks like:
(image)

Now lets write a PPM file using code:
```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main(){
    int nx = 200; //image width
    int ny = 100; //image height
    ofstream myfile;
    myfile.open ("image.ppm");
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    for(int j = ny-1; j>=0; j--){
        for(int i=0; i<nx; i++){
            clog << "\rScanlines remaining: " << (ny - j) << ' ' << flush; //used as a progres bar
            double r = (double)i/(nx-1); //red channel
            double g = (double)j/(ny-1); //green channel
            double b = 0.0; //blue channel
            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    clog << "\rDone.                 \n";
    myfile.close();
}
```

running the following command in the directory containig your file:
```bash
g++ <filename>.cpp \
&& ./a.out \
&& code image.ppm
```

Outputs  the following image:

![img1](./output/img1.png)

The code lines:
```cpp
double r = (double)i/(nx-1); //red channel
double g = (double)j/(ny-1); //green channel
double b = 0.0; //blue channel
```
define the gradient logic and you can try playing around with this. For example:

```cpp
int center_x = nx/2;
int center_y = ny/2;
double max_dist = sqrt(nx/2 * nx/2 + ny/2 * ny/2);
for(int j = ny-1; j>=0; j--){
    for(int i=0; i<nx; i++){
        int dist = sqrt((i-center_x)*(i-center_x) + (j-center_y)*(j-center_y));
        double r = dist/max_dist; //red channel
        double g = 1.0-(dist/max_dist); //green channel
        double b = 1.0; //blue channel
        ...
    }
}
```
outputs this circular gradient:

![alt text](./output/img2.png)


## The vec3 Class

We will be creating a class called vec3 the objects of which stores three integer values corresponding to rgb. This is my `vec3.h` file:

```cpp
#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec3 {
public:
    vec3() {}
    vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    inline const vec3& operator+() const { return *this; }
    inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }

    inline vec3& operator+=(const vec3 &v2);
    inline vec3& operator-=(const vec3 &v2);
    inline vec3& operator*=(const vec3 &v2);
    inline vec3& operator/=(const vec3 &v2);
    inline vec3& operator*=(const float t);
    inline vec3& operator/=(const float t);

    inline float length() const { return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]); }
    inline float squared_length() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }
    inline void make_unit_vector();

    float e[3];
};
inline std::istream& operator>>(std::istream &is, vec3 &t) {
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &t) {
    os << t.e[0] << " " << t.e[1] << " " << t.e[2];
    return os;
}

inline void vec3::make_unit_vector() {
    float k = 1.0 / sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
    e[0] *= k; e[1] *= k; e[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(float t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator/(vec3 v, float t) {
    return vec3(v.e[0]/t, v.e[1]/t, v.e[2]/t);
}

inline vec3 operator*(const vec3 &v, float t) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline float dot(const vec3 &v1, const vec3 &v2) {
    return v1.e[0] *v2.e[0] + v1.e[1] *v2.e[1]  + v1.e[2] *v2.e[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
                v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2],
                v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline vec3& vec3::operator+=(const vec3 &v){
    e[0]  += v.e[0];
    e[1]  += v.e[1];
    e[2]  += v.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const vec3 &v){
    e[0]  *= v.e[0];
    e[1]  *= v.e[1];
    e[2]  *= v.e[2];
    return *this;
}

inline vec3& vec3::operator/=(const vec3 &v){
    e[0]  /= v.e[0];
    e[1]  /= v.e[1];
    e[2]  /= v.e[2];
    return *this;
}

inline vec3& vec3::operator-=(const vec3& v) {
    e[0]  -= v.e[0];
    e[1]  -= v.e[1];
    e[2]  -= v.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const float t) {
    e[0]  *= t;
    e[1]  *= t;
    e[2]  *= t;
    return *this;
}

inline vec3& vec3::operator/=(const float t) {
    float k = 1.0/t;

    e[0]  *= k;
    e[1]  *= k;
    e[2]  *= k;
    return *this;
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

#endif
```

It defines all the necessary constructors, operations, and methods that we will require to manipulate rgb values. Make sure to use header guards, otherwise you might run into problems like this which are a pain to debug 🙃

![alt text](image.png)

Now we can change our main function to use vec3:

```cpp
#include <iostream>
#include <fstream>
#include "vec3.h"
using namespace std;

int main(){
    int nx = 200;
    int ny = 100;
    ofstream myfile;
    myfile.open ("image.ppm");
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    for(int j = ny-1; j>=0; j--){
        for(int i=0; i<nx; i++){
            vec3 col((float)i/nx, (float)j/ny, 0.2);
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}
```

