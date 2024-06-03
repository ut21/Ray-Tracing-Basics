#include <iostream>
#include <fstream>
#include "ray.h"
#include "camera.h"
#include <math.h>
#include "perlin.h"
using namespace std;

bool hit_sphere(const vec3& center, float radius, const ray& r){
    vec3 oc  = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    return (discriminant > 0);
}

float hit_sphere_at_t(const vec3& center, float radius, const ray& r){
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return -1.0;
    }
    else {
        return (-b - sqrt(discriminant) ) / (2.0*a);
    }
}

vec3 perlin_noise_color(const vec3& p, const PerlinNoise& noise_generator) {
    // Generate Perlin noise values for RGB channels using input coordinates
    
    float r = noise_generator.noise(p.x() * 0.1, p.y() * 0.1, p.z() * 0.1); // Scale down coordinates
    float g = noise_generator.noise(p.x() * 0.1 + 1, p.y() * 0.1, p.z() * 0.1); // Offset for different color channels
    float b = noise_generator.noise(p.x() * 0.1, p.y() * 0.1 + 1, p.z() * 0.1); // Offset for different color channels

    // Scale and clamp values to ensure they are within valid color range (0-1)
    r = 1 / (1 + exp(-r));
    g = 1 / (1 + exp(-g));
    b = 1 / (1 + exp(-b));

    return vec3(r, g, b);
}

vec3 color(const ray& r) {
        float t = hit_sphere_at_t(vec3(0,0,-1), 0.5, r);
        if (t > 0.0) {
            vec3 N = perlin_noise_color(r.point_at_parameter(t), PerlinNoise());
            return N;
        }
        vec3 unit_direction = unit_vector(r.direction());
        t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}


int main(){
    int nx = 2000;
    int ny = 1000;

    ofstream myfile;
    myfile.open ("image.ppm");int ns = 10;
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    camera cam(vec3(-1, 0, 1), vec3(0,0,-1), vec3(0,1,0), 45, float(nx)/float(ny));
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