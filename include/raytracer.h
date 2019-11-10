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
        : _camera(camera), _light(light), _image(std::make_shared<Image>(0, 0)) {}

    void setScene(const Octree* scene) { _scene = scene; }

    void run(int w, int h) {
        // TODO Implement this
        _image = std::make_shared<Image>(w, h);

        // The structure of the for loop should remain for incremental rendering.
        // termination criterion: add  && _running
        // #pragma omp parallel for
        for (int y = 0; y < h && _running; ++y) {
            for (int x = 0; x < w && _running; ++x) {
                Ray ray = _camera.getRay(x, y, w, h);

                _image->setPixel(x, y, compute_pixel(ray, x, y));
            }
        }
    }

    glm::dvec3 compute_pixel(const Ray& ray, int x, int y) {
        glm::dvec3 intersect, normal;

        double min = std::numeric_limits<double>::infinity();
        glm::dvec3 color{0, 0, 0};

        std::vector<Entity*> entities = _scene->intersect(ray);
        for (Entity* entity : entities) {
            if (entity->intersect(ray, intersect, normal)) {
                const auto dist = glm::distance(ray.origin, intersect);
                if (dist < min) {
                    min = dist;
                    color = compute_shade(ray, entity->material, intersect, normal);
                }
            }
        }
        return color;
    }

    glm::dvec3
    compute_shade(const Ray& ray, const Material& mat, const glm::dvec3& i, const glm::dvec3& n) {
        // glm::dvec3 v = -glm::normalize(ray.dir);
        const auto l = glm::normalize(_light - i);
        const auto N = glm::normalize(n);
        const auto v1 = glm::max(0.0, glm::dot(N, l));
        const auto v2 = glm::max(0.0, glm::dot(-N, l));

        if (1 || v1 >= v2) {
            return mat.color * v1;
        } else {
            return (glm::dvec3{1, 1, 1} - mat.color) * v2;
        }
    }

    bool running() const { return _running; }

    void stop() { _running = false; }

    void start() { _running = true; }

    std::shared_ptr<Image> getImage() const { return _image; }

  private:
    bool _running = false;
    const Octree* _scene;
    Camera _camera;
    glm::dvec3 _light;
    std::shared_ptr<Image> _image;
};
