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
//#define USE_REFRACTION

class RayTracer {

    bool running_ = false;
    const Octree* scene_;
    Camera camera_;
    glm::dvec3 light_;
    std::shared_ptr<Image> image_;

  public:
    RayTracer() = delete;
    RayTracer(const Camera& camera, glm::dvec3 light);

    void set_scene(const Octree* scene);
    void run(int w, int h);
    glm::dvec3 compute_pixel(const Ray& ray, int max_reflections) const;
    bool running() const;
    void stop();
    void start();

    std::shared_ptr<Image> get_image() const;
};
