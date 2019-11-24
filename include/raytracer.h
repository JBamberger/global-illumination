#pragma once

#include <limits>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "camera.h"
#include "entities.h"
#include "image.h"
#include "octree.h"

#define USE_MIRROR
#define USE_REFRACTION

class RayTracer {
  public:
    RayTracer() = delete;

    RayTracer(const Camera& camera, glm::dvec3 light)
        : camera_(camera), light_(light), image_(std::make_shared<Image>(0, 0))
    {
    }

    void setScene(const octree* scene) { scene_ = scene; }

    void run(int w, int h)
    {
        image_ = std::make_shared<Image>(w, h);
        camera_.set_window_size(w, h);
// The structure of the for loop should remain for incremental rendering.
// termination criterion: add  && _running
#pragma omp parallel for schedule(dynamic, 4)
        for (auto y = 0; y < h; ++y) {
            for (auto x = 0; x < w && running_; ++x) {
                const auto ray = camera_.get_ray(x, y);
                const auto pix = compute_pixel(ray, 3);
#pragma omp critical
                {
                    image_->setPixel(x, y, pix);
                }
            }
        }
    }

    glm::dvec3 compute_pixel(const Ray& ray, int max_reflections) const
    {
        if (max_reflections <= 0)
            return {0, 0, 0};

        glm::dvec3 intersect, normal; // values at minimum
        entity* min_ent = nullptr;

        {
            auto min = std::numeric_limits<double>::infinity();
            glm::dvec3 i, n; // working variables
            const auto entities = scene_->intersect(ray);

            // find closest intersecting entity
            for (auto entity : entities) {
                if (entity->intersect(ray, i, n)) {
                    const auto dist = glm::distance(ray.origin, i);
                    if (dist < min) {
                        min = dist;
                        normal = n;
                        intersect = i;
                        min_ent = entity;
                    }
                }
            }
        }

        glm::dvec3 color{0, 0, 0}; // background color

        if (min_ent == nullptr) {
            return color;
        }

        // compute light and normal vector at the intersection point
        const auto l = glm::normalize(light_ - intersect);
        normal = glm::normalize(normal);

        auto blocked = false;
        {
            const auto shadow_ray = Ray::offset_ray(intersect, l);
            const auto entities = scene_->intersect(shadow_ray);
            glm::dvec3 i, n; // working variables
            for (auto entity : entities) {
                if (entity->intersect(shadow_ray, i, n)) {
                    blocked = true;
                    break;
                }
            }
        }

        const auto mat = min_ent->material;
        const auto color_at_intersect = min_ent->get_color_at_intersect(intersect);

        // ambient: L_a = k_a * I_a
        color = color_at_intersect * mat->ambient;

        if (!blocked) {

            const auto v = glm::normalize(-ray.dir); // eye direction

            // diffuse:  L_d = k_d * I * max(0.0, dot(n, l))
            const auto diffuse = glm::dot(normal, l);
            if (diffuse > 0) {
                color += color_at_intersect * mat->diffuse * diffuse;
            }

            // specular (Blinn-Phong): L_s = k_s * I * max(0.0, dot(n,normalize(v + l)))^p
            const auto bisector = glm::normalize(v + l); // center between view and light
            const auto base = glm::dot(normal, bisector);
            if (base > 0) {
                color += mat->specular * glm::pow(base, mat->specular_exponent);
            }

#ifdef USE_MIRROR
            // mirror: L_m = k_m * trace(Ray(P, r))
            if (mat->reflective > 0) {
                // reflection direction: r = 2n * dot(n,v) - v === glm::reflect(-v, n)
                const auto reflect_dir = glm::reflect(-v, normal);
                const auto mirror_ray = Ray::offset_ray(intersect, reflect_dir);
                // Trace another ray in reflection direction
                const auto mirror = compute_pixel(mirror_ray, max_reflections - 1);

                color += mat->reflective * mirror;
            }
#endif
#ifdef USE_REFRACTION
            // glazed: (trace another ray in refraction direction)
            // It is not clear what should happen in the intersection of two objects. Which
            // material / refractive index is used?

            if (mat->refractive > 0) {
                const auto entering = glm::dot(normal, ray.dir);

                // if entering == false -> divide by 1.0 for air
                const auto eta =
                    entering ? ray.refractive_index / mat->refractive_index : mat->refractive_index;

                const auto refraction_direction = glm::refract(ray.dir, normal, eta);
                auto refract_ray = Ray::offset_ray(intersect, refraction_direction);

                // reset the index if we leave an object
                refract_ray.refractive_index = entering ? mat->refractive_index : 1.0;

                const auto refraction = compute_pixel(refract_ray, max_reflections - 1);
                color += mat->refractive * refraction;
            }
#endif
        }

        // the color value must be clamped because otherwise high illumination will produce
        // values above 1. This results in errors from Qt.
        color = glm::clamp(color, 0., 1.);

        return color;
    }

    bool running() const { return running_; }

    void stop() { running_ = false; }

    void start() { running_ = true; }

    std::shared_ptr<Image> getImage() const { return image_; }

  private:
    bool running_ = false;
    const octree* scene_;
    Camera camera_;
    glm::dvec3 light_;
    std::shared_ptr<Image> image_;
};
