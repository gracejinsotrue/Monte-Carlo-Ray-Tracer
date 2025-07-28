#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"
#include "rtweekend.h"
#include "aabb.h"    // For bounding box
#include <algorithm> // For std::min, std::max
#include <cmath>

class triangle : public hittable
{
public:
    triangle(const point3 &v0, const point3 &v1, const point3 &v2, shared_ptr<material> mat)
        : v0(v0), v1(v1), v2(v2), mat(mat)
    {
        // pre-compute edges and normal for efficiency
        edge1 = v1 - v0;
        edge2 = v2 - v0;
        normal = unit_vector(cross(edge1, edge2));

        // compute bounding box for BVH
        bbox = aabb(
            point3(std::min({v0.x(), v1.x(), v2.x()}),
                   std::min({v0.y(), v1.y(), v2.y()}),
                   std::min({v0.z(), v1.z(), v2.z()})),
            point3(std::max({v0.x(), v1.x(), v2.x()}),
                   std::max({v0.y(), v1.y(), v2.y()}),
                   std::max({v0.z(), v1.z(), v2.z()})));
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // Möller-Trumbore ray-triangle intersection algorithm, this is the gold standard for ray-triangle intersection
        // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
        // I have yet to fully understand it too
        const double EPSILON = 1e-8;
        vec3 h = cross(r.direction(), edge2);
        double a = dot(edge1, h);

        // ray is parallel to triangle
        if (a > -EPSILON && a < EPSILON)
            return false;

        double f = 1.0 / a;
        vec3 s = r.origin() - v0;
        double u = f * dot(s, h);

        // check if intersection point is outside triangle
        if (u < 0.0 || u > 1.0)
            return false;

        vec3 q = cross(s, edge1);
        double v = f * dot(r.direction(), q);

        if (v < 0.0 || u + v > 1.0)
            return false;

        // compute intersection distance
        double t = f * dot(edge2, q);

        if (!ray_t.surrounds(t))
            return false;

        // We have a valid intersection!
        rec.t = t;
        rec.p = r.at(t);
        rec.mat = mat;

        // Set normal (handle front/back face)
        vec3 outward_normal = normal;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

    // Essential for BVH construction
    aabb bounding_box() const { return bbox; }

private:
    point3 v0, v1, v2;        // Triangle vertices
    vec3 edge1, edge2;        // Pre-computed edges (v1-v0, v2-v0)
    vec3 normal;              // pre-computed normal
    shared_ptr<material> mat; // manage object lifietime using reference counting
    aabb bbox;                // bounding box for this triangle
};

#endif