//main.cc
#include "header/macro.h"
#include "header/hittable_list.h"
#include "header/sphere.h"
#include "header/camera.h"
#include "header/vec3.h"
#include "header/ray.h"
#include "header/material.h"
#include <iostream>

vec3 ray_color(const ray& r, const hittable& world, int depth) {

    if(depth <= 0){                                                             // recursion exit
        return vec3(0.0, 0.0, 0.0);
    }

    hit_record rec;

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        vec3 attenuation;
        if(rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
            return attenuation * ray_color(scattered, world, depth-1);
        }
        return vec3(0.0, 0.0, 0.0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);                // lerp background
}

int main() {

    const int image_width = 200;
    const int image_height = 100;
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // PPM header-info

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // world generate

    hittable_list world;
    world.add(make_shared<sphere>(
        vec3(0,0,-1), 0.5, make_shared<lambertian>(vec3(0.1, 0.2, 0.5))));

    world.add(make_shared<sphere>(
        vec3(0,-100.5,-1), 100, make_shared<lambertian>(vec3(0.8, 0.8, 0.0))));

    world.add(make_shared<sphere>(
        vec3(1,0,-1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.3)));
    world.add(make_shared<sphere>(
        vec3(-1,0,-1), 0.5, make_shared<dielectric>(1.5)));
        world.add(make_shared<sphere>(
        vec3(-1,0,-1), -0.45, make_shared<dielectric>(1.5)));

    // camera setting
    const auto aspect_ratio = double(image_width) / image_height;
    camera cam(vec3(0, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0), 90, aspect_ratio);

    // draw_call
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 color(0.0, 0.0, 0.0);
            for(int s = 0; s < samples_per_pixel; ++s){
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);
                color += ray_color(r, world, max_depth);
            }
            color.write_color(std::cout, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}