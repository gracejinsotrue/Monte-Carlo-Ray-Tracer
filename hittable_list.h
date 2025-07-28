
// container that manages multiple 3D objects in a ray tracing scene.
// acts as a composite pattern implementation that treats a collection of objects as a single hittable entity.

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include "aabb.h"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr; // use a shard poin rfor memory maangemenw

class hittable_list : public hittable
{
public:
    std::vector<shared_ptr<hittable>> objects; // stores all scene objcs

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) // adds objects to the scene
    {
        objects.push_back(object);
    }
    // this finds closest intersection amonst all objcts:
    //  1) itrate ethrough all objects in the scene
    // 2) test each object for intersection
    // 3) track the closest hit, update the closest so far
    // 4) closest so far gets set as the furthest ray so we can progresively narrow
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto &object : objects)
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    aabb bounding_box() const override
    {
        if (objects.empty())
            return aabb();

        aabb result = objects[0]->bounding_box();
        for (size_t i = 1; i < objects.size(); i++)
        {
            result = aabb::surrounding_box(result, objects[i]->bounding_box());
        }
        return result;
    }
};

#endif