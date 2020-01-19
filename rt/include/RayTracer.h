#pragma once

#include <memory>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Octree.h"
#include "camera.h"
#include "image.h"

class RayTracer {

    bool running_ = false;

    Camera camera_;
    std::shared_ptr<const Octree> scene_;
    std::shared_ptr<Image> image_;

    std::default_random_engine rng_;
    std::uniform_real_distribution<double> dist01_;
    std::uniform_real_distribution<double> dist11_;

  public:
    RayTracer() = delete;
    explicit RayTracer(const Camera& camera, std::shared_ptr<const Octree> scene);

    void setScene(std::shared_ptr<const Octree> scene);
    void run(int w, int h);
    bool running() const;
    void stop();
    void start();

    std::shared_ptr<Image> get_image() const;

  private:
    glm::dvec3 computePixel(int x, int y);
    glm::dvec3 computePixel(const Ray& ray) const;
    glm::dvec3 hemisphere(glm::dvec3 normal, glm::dvec3 direction);
};
