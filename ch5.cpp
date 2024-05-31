#include <iostream>
#include <fstream>
#include "sphere.h"
#include "hitablelist.h"
#include "float.h"
#include "camera.h"
#include "random.h"
#include "material.h"
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"

using namespace std;

vec3 color(const ray& r, hitable *world, int depth) {
        hit_record rec;
        if (world->hit(r, 0.001, MAXFLOAT, rec)) {
            ray scattered;
            vec3 attenuation;
            if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
                return attenuation*color(scattered, world, depth+1);
            }
            else {
                return vec3(0,0,0);
            }
        }
        else {
            vec3 unit_direction = unit_vector(r.direction());
            float t = 0.5*(unit_direction.y() + 1.0);
            return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
        }
    }

hitable* random_scene(){
    int n = 500;
    hitable** list = new hitable*[n+1];
    list[0] = new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for(int a = -11; a < 11; a++){
        for(int b = -11; b < 11; b++){
            float choose_mat = random_double();
            vec3 center(a+0.9*random_double(), 0.2, b+0.9*random_double());
            if((center-vec3(4,0.2,0)).length() > 0.9){
                if(choose_mat < 0.8){
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(random_double()*random_double(), random_double()*random_double(), random_double()*random_double())));
                }
                else if(choose_mat < 0.95){
                    list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1 + random_double()), 0.5*(1 + random_double()), 0.5*(1 + random_double()))));
                }
                else{
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    
    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5)));

    return new hitable_list(list, i);
}

int main(){
    int nx = 200;
    int ny = 100;
    int ns = 100;
    ofstream myfile;
    myfile.open ("output.ppm");
    myfile << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *world = random_scene();
    camera cam(vec3(-2,2,1), vec3(0,0,-1), vec3(0,1,0),90, float(nx)/float(ny));
    for(int j = ny-1; j>=0; j--){
        for(int i=0; i<nx; i++){
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                    float u = float(i + random_double()) / float(nx);
                    float v = float(j + random_double()) / float(ny);
                    ray r = cam.get_ray(u, v);
                    vec3 p = r.point_at_parameter(2.0);
                    col += color(r, world, 0);
                }
                col /= float(ns);
                col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            myfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    myfile.close();
}