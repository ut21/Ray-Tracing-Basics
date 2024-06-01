// #ifndef CAMERAH
//     #define CAMERAH

//     #include "ray.h"

//     class camera {
//         public:
//             camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect) {
//                 // vfov is top to bottom in degrees
//                 vec3 u, v, w;
//                 float theta = vfov*M_PI/180;
//                 float half_height = tan(theta/2);
//                 float half_width = aspect * half_height;
//                 origin = lookfrom;
//                 w = unit_vector(lookfrom - lookat);
//                 u = unit_vector(cross(vup, w));
//                 v = cross(w, u);
//                 lower_left_corner = origin - half_width*u - half_height*v - w;
//                 horizontal = 2*half_width*u;
//                 vertical = 2*half_height*v;
//             }
//             ray get_ray(float s, float t) {
//                 return ray(origin,
//                            lower_left_corner + s*horizontal + t*vertical - origin);
//             }

//             vec3 origin;
//             vec3 lower_left_corner;
//             vec3 horizontal;
//             vec3 vertical;
//     };
//     #endif

#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

class camera {
    public:
        // Default constructor
        camera() {
            // hardcoding the boundaries of our view
            lower_left_corner = vec3(-2.0, -1.0, -1.0);
            horizontal = vec3(4.0, 0.0, 0.0);
            vertical = vec3(0.0, 2.0, 0.0);
            // hardcoding the camera position
            origin = vec3(0.0, 0.0, 0.0);
        }

        // Parameterized constructor
        camera(const vec3& lower_left_corner, const vec3& horizontal, const vec3& vertical, const vec3& origin)
            : lower_left_corner(lower_left_corner), horizontal(horizontal), vertical(vertical), origin(origin) {}

        // Outputs a ray for a pixel
        ray get_ray(float u, float v) const {
            return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
        }

        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
};
#endif
