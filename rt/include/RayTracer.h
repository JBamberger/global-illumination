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
    size_t samples_;
    Camera camera_;
    std::shared_ptr<const Octree> scene_;
    std::shared_ptr<Image> image_;

  public:
    RayTracer() = delete;
    explicit RayTracer(const Camera& camera, std::shared_ptr<const Octree> scene);

    void setScene(std::shared_ptr<const Octree> scene);
    void setSampleCount(size_t samples);
    void run(int w, int h);
    bool running() const;
    void stop();
    void start();

    std::shared_ptr<Image> getImage() const;

  private:
    /**
     * Iterative implementation of the path tracing.
     *
     * @param x x coordinate of the current pixel
     * @param y y coordinate of the current pixel
     * @return the light intensity transported on the traced path
     */
    glm::dvec3 computePixel(int x, int y) const;

    /**
     * Recursive implementation of the path tracing.
     *
     * @param ray input ray
     * @return The light intensity transported on the traced path
     */
    glm::dvec3 computePixel(const Ray& ray) const;
};
