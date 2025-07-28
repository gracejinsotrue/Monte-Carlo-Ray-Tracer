#ifndef AABB_H
#define AABB_H

#include "vec3.h"
#include "ray.h"
#include "interval.h"
#include <algorithm> // for std::swap, std::min, std::max
#include <cmath>     // for math functions

// AXIS-ALIGNED BOUNDING BOX - THIS IS NECESSARY!! for BVH acceleration
class aabb
{
public:
    interval x, y, z; // intervals for each axis

    aabb() = default; // defaults to empty box

    aabb(const interval &x, const interval &y, const interval &z)
        : x(x), y(y), z(z) {}

    aabb(const point3 &a, const point3 &b)
    {
        // Construct AABB from two corner points
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    // get the interval for a specific axis, where 0=x, 1=y, 2=z
    const interval &axis_interval(int n) const
    {
        if (n == 1)
            return y;
        if (n == 2)
            return z;
        return x;
    }

    // fast ray-box intersection test, this is the heart of BVH efficiency!
    bool hit(const ray &r, interval ray_t) const
    {
        auto invD = 1.0 / r.direction().x();
        auto t0 = (x.min - r.origin().x()) * invD;
        auto t1 = (x.max - r.origin().x()) * invD;

        if (invD < 0.0)
            std::swap(t0, t1);

        auto tmin = std::max(t0, ray_t.min);
        auto tmax = std::min(t1, ray_t.max);

        if (tmin >= tmax)
            return false;

        // Y axis
        invD = 1.0 / r.direction().y();
        t0 = (y.min - r.origin().y()) * invD;
        t1 = (y.max - r.origin().y()) * invD;

        if (invD < 0.0)
            std::swap(t0, t1);

        tmin = std::max(t0, tmin);
        tmax = std::min(t1, tmax);

        if (tmin >= tmax)
            return false;

        // Z axis
        invD = 1.0 / r.direction().z();
        t0 = (z.min - r.origin().z()) * invD;
        t1 = (z.max - r.origin().z()) * invD;

        if (invD < 0.0)
            std::swap(t0, t1);

        tmin = std::max(t0, tmin);
        tmax = std::min(t1, tmax);

        return tmin < tmax;
    }

    // return the longest axis (0=x, 1=y, 2=z)
    // Used for BVH splitting decisions
    int longest_axis() const
    {
        if (x.size() > y.size())
            return (x.size() > z.size()) ? 0 : 2;
        else
            return (y.size() > z.size()) ? 1 : 2;
    }

    // combine two bounding boxes
    static aabb surrounding_box(const aabb &box0, const aabb &box1)
    {
        return aabb(
            interval(std::min(box0.x.min, box1.x.min), std::max(box0.x.max, box1.x.max)),
            interval(std::min(box0.y.min, box1.y.min), std::max(box0.y.max, box1.y.max)),
            interval(std::min(box0.z.min, box1.z.min), std::max(box0.z.max, box1.z.max)));
    }

    // get the center point of the bounding box
    point3 center() const
    {
        return point3(
            (x.min + x.max) * 0.5,
            (y.min + y.max) * 0.5,
            (z.min + z.max) * 0.5);
    }
};

#endif