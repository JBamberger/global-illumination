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
        : camera_(camera), light_(light), image_(std::make_shared<Image>(0, 0)) {}

    void setScene(const Octree* scene) { scene_ = scene; }

    void run(int w, int h) {
        image_ = std::make_shared<Image>(w, h);

        // The structure of the for loop should remain for incremental rendering.
        // termination criterion: add  && _running
        // #pragma omp parallel for
        for (auto y = 0; y < h && running_; ++y) {
            for (auto x = 0; x < w && running_; ++x) {
                const auto ray = camera_.getRay(x, y, w, h);

                image_->setPixel(x, y, compute_pixel(ray, x, y));
            }
        }
    }

    glm::dvec3 compute_pixel(const Ray& ray, int x, int y) const {
        glm::dvec3 intersect, normal;

        auto min = std::numeric_limits<double>::infinity();
        glm::dvec3 color{0, 0, 0};

        auto entities = scene_->intersect(ray);
        for (auto entity : entities) {
            if (entity->intersect(ray, intersect, normal)) {
                const auto dist = glm::distance(ray.origin, intersect);
                if (dist < min) {
                    min = dist;
                    // compute light and normal vector at the intersection point
                    const auto l = glm::normalize(light_ - intersect);
                    const auto n = glm::normalize(normal);

                    // apply diffuse shading
                    color = entity->material.color * glm::max(0.0, glm::dot(n, l));
                }
            }
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
