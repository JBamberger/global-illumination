#pragma once

#include <limits>
#include <memory>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Octree.h"
#include "camera.h"
#include "entities.h"
#include "image.h"

class RayTracer {

    bool running_ = false;
    const Octree* scene_;
    Camera camera_;
    glm::dvec3 light_;
    std::shared_ptr<Image> image_;
    std::default_random_engine rng_;
    std::uniform_real_distribution<double> dist01_;
    std::uniform_real_distribution<double> dist11_;

  public:
    RayTracer() = delete;
    RayTracer(const Camera& camera, glm::dvec3 light);

    void set_scene(const Octree* scene);
    void run(int w, int h);
    bool running() const;
    void stop();
    void start();

    std::shared_ptr<Image> get_image() const;

  private:
    glm::dvec3 computePixel(int x, int y);
    glm::dvec3 computePixel(const Ray& ray);
    glm::dvec3 hemisphere(glm::dvec3 normal, glm::dvec3 direction);
};
