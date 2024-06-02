#include <iostream>
#include <fstream>
#include "ray.h"
#include "camera.h"
#include <math.h>
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

vec3 color(const ray& r) {
    float t = hit_sphere_at_t(vec3(0,0,-1), 0.5, r);
    if (t != -1) {
        vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0,0,-1)); //(0,0,-1) is the hard coded centre of the sphere
        return 0.5*vec3(N.z()+1, N.x()+1, N.y()+1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

float hit_cylinder_at_t(const vec3& center, float radius, float z_min, float z_max, const ray& r){
    vec3 oc = r.origin() - center;
    vec3 direction = r.direction();
    float bx = dot(direction, vec3(1,0,0));
    float by = dot(direction, vec3(0,1,0));
    float bz = dot(direction, vec3(0,0,1));
    float ax = dot(oc, vec3(1,0,0));
    float ay = dot(oc, vec3(0,1,0));
    float az = dot(oc, vec3(0,0,1));
    float a = bx*bx + by*by;
    float b = 2.0f * (ax*bx + ay*by);
    float c = ax*ax + ay*ay - radius * radius;
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0) {
        return -1.0f;
    } else {
        float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
        float t1 = (-b + sqrt(discriminant)) / (2.0f * a);

        float z0 = dot(r.point_at_parameter(t0), vec3(0,0,1));
        float z1 = dot(r.point_at_parameter(t1), vec3(0,0,1));

        if (z0 > z_min && z0 < z_max) {
            return t0;
        } else if (z1 > z_min && z1 < z_max) {
            return t1;
        } else {
            return -1.0f;
        }
    }
}

vec3 colorCylinder(const ray& r) {
    float t = hit_cylinder_at_t(vec3(0,0,-1), 0.5, -2.5, 2.5, r);
    if (t != -1) {
        vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0,0,-1)); //(0,0,-1) is the hard coded centre of the sphere
        return 0.5*vec3(N.z()+1, N.x()+1, N.y()+1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

float hit_cone_at_t(const vec3& center, float radius, float height, const ray& r) {
    vec3 oc = r.origin() - center;
    vec3 direction = r.direction();

    float bx = dot(direction, vec3(1,0,0));
    float by = dot(direction, vec3(0,1,0));
    float bz = dot(direction, vec3(0,0,1));
    float ax = dot(oc, vec3(1,0,0));
    float ay = dot(oc, vec3(0,1,0));
    float az = dot(oc, vec3(0,0,1));
    
    float k = radius / height;
    float k2 = k * k;
    
    float a = bx*bx + by*by -k2*bz*bz;
    float b = 2*(ax*bx + ay*by - k2*az*bz);
    float c = ax*ax + ay*ay - k2*az*az;
    
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0) {
        return -1.0f; // No intersection
    } else {
        float sqrt_disc = sqrt(discriminant);
        float t0 = (-b - sqrt_disc) / (2.0f * a);
        float t1 = (-b + sqrt_disc) / (2.0f * a);

        return t0;
        
        // float z0 = dot(r.point_at_parameter(t0), vec3(0,0,1));
        // float z1 = dot(r.point_at_parameter(t1), vec3(0,0,1));
        
        // bool t0_valid = (z0 >= z_min && z0 <= z_max);
        // bool t1_valid = (z1 >= z_min && z1 <= z_max);
        
        // if (t0_valid && t1_valid) {
        //     return min(t0, t1); // Return the closest valid intersection
        // } else if (t0_valid) {
        //     return t0;
        // } else if (t1_valid) {
        //     return t1;
        // } else {
        //     return -1.0f; // No valid intersection within bounds
        // }
    }
}

vec3 colorCone(const ray& r) {
    float t = hit_cone_at_t(vec3(0,0,-1), 0.5, 1.0, r);
    if (t != -1) {
        vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0,0,-1)); //(0,0,-1) is the hard coded centre of the sphere
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
    camera cam((1/sqrt(40))*vec3(0,3,-5), vec3(0,0,-1), vec3(0,1,0), 90, float(nx)/float(ny));
    for(int j = ny-1; j>=0; j--){
        for(int i=0; i<nx; i++){
            float u = float(i) / float(nx);
            float v = float(j) / float(ny);
            ray r = cam.get_ray(u, v);
            vec3 col = colorCone(r);
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}