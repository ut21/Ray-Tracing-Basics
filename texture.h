#include "vec3.h"
#include "perlin.h"
#include "color.h"

class texture {
  public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const vec3& p) const = 0;
};

class constant_texture : public texture {
    public:
        constant_texture() { }
        constant_texture(vec3 c) : color(c) { }
        virtual vec3 value(float u, float v, const vec3& p) const {
            return color;
        }
        vec3 color;
};

class noise_texture : public texture {
  public:
    noise_texture() {}

    color value(double u, double v, const vec3& p) const override {
        return color(1,1,1) * noise.noise(p);
    }

  private:
    perlin noise;
};