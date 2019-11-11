#pragma once

#include <algorithm>
#include <memory>
//#include <future>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "camera.h"
#include "entities.h"
#include "image.h"
#include "octree.h"

class RayTracer {
  public:
    RayTracer() = delete;

    RayTracer(const Camera& camera, glm::dvec3 light)
        : camera_(camera), light_(light), image_(std::make_shared<Image>(0, 0))
    {
    }

    void setScene(const Octree* scene) { scene_ = scene; }

    void run(int w, int h)
    {
        image_ = std::make_shared<Image>(w, h);

        // The structure of the for loop should remain for incremental rendering.
        // termination criterion: add  && _running
        // #pragma omp parallel for
        for (auto y = 0; y < h && running_; ++y) {
            for (auto x = 0; x < w && running_; ++x) {
                const auto ray = camera_.getRay(x, y, w, h);

                image_->setPixel(x, y, compute_pixel(ray, 3));
            }
        }
    }

    glm::dvec3 compute_pixel(const Ray& ray, int max_reflections) const
    {
        if (max_reflections <= 0)
            return {0, 0, 0};

        glm::dvec3 i, n; // working variables

        auto min = std::numeric_limits<double>::infinity();
        glm::dvec3 intersect, normal; // values at minimum
        entity* min_ent = nullptr;

        // find closest intersecting entity
        auto entities = scene_->intersect(ray);
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

        glm::dvec3 color{0, 0, 0};
        if (min_ent != nullptr) {
            // compute light and normal vector at the intersection point
            const auto l = glm::normalize(light_ - intersect);
            normal = glm::normalize(normal);

            // the offset value starts the ray slightly in the direction of the light to avoid
            // intersecting the entity itself.
            const auto offset = 1e-7;
            const auto shadow_ray = Ray{intersect + l * offset, l};
            entities = scene_->intersect(shadow_ray);

            auto blocked = false;
            for (auto entity : entities) {
                if (entity->intersect(shadow_ray, i, n)) {
                    blocked = true;
                    break;
                }
            }

#define USE_AMBIENT
#define USE_DIFFUSE
#define USE_SPECULAR
#define USE_MIRROR

#ifdef USE_AMBIENT
            color = min_ent->material.color * min_ent->material.ambient;
#endif

            if (!blocked) {
                // ambient:
                // L_a = k_a * I_a
                // diffuse:
                // L_d = k_d * I * glm::max(0.0, glm::dot(n, l))
                // specular:
                // h = glm::normalize(v + l)
                // L_s = k_s * I * glm::pow(glm::max(0.0, glm::dot(n,h)), p)
                // mirror: (trace another ray in reflection direction)
                // r = 2n(dot(n,v)) - v
                // L_m = Ray(P, r).color
                // glazed:
                // L = L_a + L_d + L_m
                //
                // For many light sources:
                // L = L_a + sum(L_d(i) + L_s(i))

                // eye direction
                const auto v = glm::normalize(-ray.dir);

#ifdef USE_DIFFUSE
                const auto diffuse = glm::max(0.0, glm::dot(normal, l));
                color += min_ent->material.color * min_ent->material.diffuse * diffuse;
#endif
#ifdef USE_SPECULAR
                // center between view and light
                const auto h = glm::normalize(v + l);
                const auto specular = glm::pow(glm::max(0.0, glm::dot(normal, h)),
                                               min_ent->material.specular_exponent);
                color += min_ent->material.specular * specular;
#endif
#ifdef USE_MIRROR
                if (min_ent->material.glazed > 0) {
                    // reflection direction
                    const auto r = 2. * normal * glm::dot(normal, v) - v;
                    const auto mirror_ray = Ray{intersect, r};
                    const auto mirror = compute_pixel(mirror_ray, max_reflections - 1);

                    color += min_ent->material.glazed * mirror;
                }
#endif

                // apply diffuse shading
            }
            color = glm::clamp(color, 0., 1.);
        }

        return color;
    }

    bool running() const { return running_; }

    void stop() { running_ = false; }

    void start() { running_ = true; }

    std::shared_ptr<Image> getImage() const { return image_; }

  private:
    bool running_ = false;
    const Octree* scene_;
    Camera camera_;
    glm::dvec3 light_;
    std::shared_ptr<Image> image_;
};
