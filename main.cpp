#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "triangle.h"
#include "bvh.h"
#include <chrono>

void create_impressive_scene(hittable_list &world)
{
    // Ground
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
    // final render for now! we will make a lot of random spheres and render theem
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz); // aside from color, add fuzziness paameter to the metals
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else
                {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // three hero spheres
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // add in trianglse because why not
    auto triangle_material = make_shared<metal>(color(0.8, 0.3, 0.3), 0.1);

    // a few triangular "sails" or "fins"
    world.add(make_shared<triangle>(
        point3(2, 0, 2), point3(3, 2, 2), point3(2, 2, 3), triangle_material));
    world.add(make_shared<triangle>(
        point3(-2, 0, 2), point3(-3, 2, 2), point3(-2, 2, 3), triangle_material));
}

int main()
{
    // World
    hittable_list world;

    // create an impressive scene with many objects
    create_impressive_scene(world);

    std::cerr << "Scene created with " << world.objects.size() << " objects" << std::endl;

    // Build BVH
    std::cerr << "Building BVH..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    auto bvh_world = make_shared<bvh_node>(world);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cerr << "BVH built in " << duration.count() << " ms" << std::endl;

    // Camera
    camera cam;
    // cam.aspect_ratio = 16.0 / 9.0;
    // cam.image_width = 400;
    // cam.samples_per_pixel = 100;
    // cam.max_depth = 50; // use this depth limit to avoid recursing

    // cam.vfov = 90; // fatass, changing ov will zoom in or out (largre is zooming out!)
    // cam.lookfrom = point3(-2, 2, 1);
    // cam.lookat = point3(0, 0, -1);
    // cam.vup = vec3(0, 1, 0);

    // cam.defocus_angle = 10.0;
    // cam.focus_dist = 3.4;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 800;      // Higher resolution
    cam.samples_per_pixel = 50; // change samples per pixel to larger numbr for mor accurate image, but for runtime purposes and my laptop not dying, let's go with this for now
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3); // Better camera position
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6; // Add depth of field
    cam.focus_dist = 10.0;

    std::cerr << "Starting render..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();

    // Render with BVH
    cam.render(*bvh_world);

    end_time = std::chrono::high_resolution_clock::now();
    auto render_duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cerr << "Render completed in " << render_duration.count() << " seconds" << std::endl;

    return 0;
}