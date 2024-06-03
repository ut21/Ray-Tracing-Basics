#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include "ray.h"
#include "camera.h"
#include <math.h>
#include <complex>
using namespace std;

struct CubicCoefficients {
    float a;
    float b;
    float c;
    float d;
};

CubicCoefficients findCubicCoefficients(const ray& r) {
    vec3 origin = r.origin();
    vec3 direction = r.direction();

    float x0 = origin.x();
    float y0 = origin.y();
    float z0 = origin.z();
    float dx = direction.x();
    float dy = direction.y();
    float dz = direction.z();

    // Calculate the constants for the cubic equation
    float a3 = dy * dy * dy * (1 - 12 * y0);
    float a2 = 3 * dy * dy * (y0 * (1 - y0) * (1 - y0) - (dx * dx + dz * dz));
    float a1 = 2 * (x0 * dx + z0 * dz) - 3 * dy * (1 - y0) * (y0 * (1 - y0) * (1 - y0));
    float a0 = x0 * x0 + z0 * z0 - (y0 * y0 * y0 * (1 - y0) * (1 - y0) * (1 - y0));

    CubicCoefficients coeffs = {a3, a2, a1, a0};
    return coeffs;
}

complex<float> cbrt(complex<float> z) {
    float r = abs(z);
    float theta = arg(z);
    return polar(cbrt(r), theta / 3);
}

float solveCubic(CubicCoefficients coeffs) {

    float a = coeffs.a;
    float b = coeffs.b;
    float c = coeffs.c;
    float d = coeffs.d;

    // Normalize coefficients
    float p = (3 * a * c - b * b) / (3 * a * a);
    float q = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) / (27 * a * a * a);
    float discriminant = (q / 2) * (q / 2) + (p / 3) * (p / 3) * (p / 3);

    vector<float> realRoots;

    if (discriminant > 0) {
        // One real root
        float u = std::cbrt(-q / 2 + std::sqrt(discriminant));
        float v = std::cbrt(-q / 2 - std::sqrt(discriminant));
        float realRoot = u + v - b / (3 * a);
        realRoots.push_back(realRoot);
    } else if (discriminant == 0) {
        // Two or three real roots, at least two are equal
        float u = std::cbrt(-q / 2);
        realRoots.push_back(2 * u - b / (3 * a));
        realRoots.push_back(-u - b / (3 * a));
    } else {
        // Three distinct real roots
        float r = std::sqrt(-(p / 3) * (p / 3) * (p / 3));
        float phi = std::acos(-q / (2 * r));
        float sqrt_p_3 = 2 * std::sqrt(-p / 3);
        realRoots.push_back(sqrt_p_3 * std::cos(phi / 3) - b / (3 * a));
        realRoots.push_back(sqrt_p_3 * std::cos((phi + 2 * M_PI) / 3) - b / (3 * a));
        realRoots.push_back(sqrt_p_3 * std::cos((phi + 4 * M_PI) / 3) - b / (3 * a));
    }

    // Return the smallest real root
    if (!realRoots.empty()) {
        return *std::min_element(realRoots.begin(), realRoots.end());
    } else {
        return -1; // This case won't happen for cubic equations
    }
}


float hit_surface_at_t(const ray& r) {
    float roots = solveCubic(findCubicCoefficients(r));
    cout << roots << endl;
    return roots;
}

vec3 compute_normal(const vec3& point) {
    float x = point.x();
    float y = point.y();
    float z = point.z();

    float fx = 2 * x;
    float fy = -3 * y * y * (1 - y) * (1 - y) + y * y * y * (3 - 6 * y);
    float fz = 2 * z;

    vec3 normal(fx, fy, fz);
    float magnitude = std::sqrt(normal.x() * normal.x() + normal.y() * normal.y() + normal.z() * normal.z());
    normal /= magnitude;

    return normal;
}

vec3 color(const ray& r) {
    float t = hit_surface_at_t(r);
    if (t != -1) {
        vec3 N = compute_normal(r.point_at_parameter(t));
        return 0.5*vec3(N.z()+1, N.x()+1, N.y()+1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

int main(){
    int nx = 200;
    int ny = 100;
    ofstream myfile;
    myfile.open ("image.ppm");
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    camera cam(vec3(0,0.5,3), vec3(0,0.5,0), vec3(0,1,0), 150, float(nx)/float(ny));
    for(int j = ny-1; j>=0; j--){
        for(int i=0; i<nx; i++){
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r = cam.get_ray(u, v);
            vec3 col = color(r);
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}