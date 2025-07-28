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

// dielectric material that always refracts WHEN POSIBLW
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
        // vec3 refracted = refract(unit_direction, rec.normal, ri);
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
        // when a ray enters a medium of lower index of refraction at a sufficiently glancing angle, it can refract with an angle greater than 90°.
        // if a solution does not exist, the glass cannot refract, and therefore must reflect the ray:
        // givn cosθ=R⋅n, we're going to have to reflect
        bool cannot_refract = ri * sin_theta > 1.0; // refraction is limited to
        vec3 direction;
        // basically if more thana 1, refraction is not posible so lt's reflect instead

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);

        return true;
    }

private:
    // refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index)
    {
        // real glass has reflectivity that varies with nagle bu we will just use Schlick polynomial approximaiton
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};
#endif