#ifndef BVH_H
#define BVH_H

#include "hittable.h"
#include "hittable_list.h"
#include "aabb.h"
#include "triangle.h"
#include "sphere.h"
#include <algorithm>
#include <memory>

class bvh_node : public hittable
{
public:
    // construct BVH from a list of objects
    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(std::vector<shared_ptr<hittable>> &objects, size_t start, size_t end)
    {
        // build BVH recursively

        // calculate bounding box for all objects in this range
        bbox = aabb();
        for (size_t i = start; i < end; i++)
        {
            bbox = aabb::surrounding_box(bbox, get_bounding_box_static(objects[i]));
        }

        int axis = bbox.longest_axis(); // choose split axis
        size_t object_span = end - start;
        // base cases:
        if (object_span == 1)
        {
            // leaf node - single object
            left = right = objects[start];
        }
        else if (object_span == 2)
        {
            // two objects - make them children
            left = objects[start];
            right = objects[start + 1];
        }
        else
        {
            // then we have more than 2 objects - split and recurse

            // sort objects along the chosen axis
            std::sort(objects.begin() + start, objects.begin() + end,
                      [axis](const shared_ptr<hittable> &a, const shared_ptr<hittable> &b)
                      {
                          return get_bounding_box_static(a).axis_interval(axis).min <
                                 get_bounding_box_static(b).axis_interval(axis).min;
                      });

            // split in the middle
            auto mid = start + object_span / 2;
            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // we will early exit if ray doesn't hit bounding box for optimization's sake
        if (!bbox.hit(r, ray_t))
            return false;

        // Test both children
        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const { return bbox; }

private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    // helper to get bounding box from any hittable object - make static for now
    static aabb get_bounding_box_static(shared_ptr<hittable> object)
    {
        // for triangles, call bounding_box() method
        // for spheres, we need to add this method to sphere class
        if (auto triangle_ptr = dynamic_cast<triangle *>(object.get()))
        {
            return triangle_ptr->bounding_box();
        }

        // for spheres
        if (auto sphere_ptr = dynamic_cast<sphere *>(object.get()))
        {
            return sphere_ptr->bounding_box();
        }

            return aabb();
    }
};

#endif