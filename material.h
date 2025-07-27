#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "color.h"

// if we want diff objects to have diff materials, we can
//  1) produce scatered ray
// 2) if scatered, say how much ray is attenutated

// hence we will make a virtual bool scatter with attentuation and scattered

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const
    {
        return false;
    }
};

class lambertian : public material
{
public:
    lambertian(const color &albedo) : albedo(albedo) {}
    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        auto scatter_direction = rec.normal + random_unit_vector(); // how lambertian random works

        // catch degenerate scatter direction- i.e. if we have the random unit vector is exactly opposite norml vector, they will sum to 0.
        // we do not want that b/c it'll lead to infinities and/or NaNs
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo; // reduction of intensity is just the albedo, or fracitonal rflectance here
        return true;
    };

private:
    color albedo;
};

class metal : public material
{
public:
    metal(const color &albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {} // albedo property and fuzz it

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        // fuzz is just randomizing the reflectd riection by using a small sphere and chooosing a new endpoint for the ray.
        //  fuzz has to be scald compard to reflectio nreflector, so just normalize reflected ray
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

        scattered = ray(rec.p, reflected);
        attenuation = albedo;

        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    double fuzz;
};

// dielectric material that always reracs
class dielectric : public material
{
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        vec3 refracted = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, refracted);
        return true;
    }

private:
    // refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;
};
#endif