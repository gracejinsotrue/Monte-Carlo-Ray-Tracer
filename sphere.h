#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"
#include <cmath>

class sphere : public hittable
{
public:
    // finally init the sphere with a material
    sphere(const point3 &center, double radius, shared_ptr<material> mat)
        : center(center), radius(std::fmax(0, radius)), mat(mat) {}

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);

        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        rec.mat = mat;

        return true;
    }

    // add bounding box method for BVH
    aabb bounding_box() const
    {
        auto radius_vec = vec3(radius, radius, radius);
        return aabb(center - radius_vec, center + radius_vec);
    }

private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif