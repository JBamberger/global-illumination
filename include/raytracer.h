#pragma once

#include <algorithm>
#include <memory>
//#include <future>

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
        for (int y = 0; y < h && _running; ++y) {
            for (int x = 0; x < w && _running; ++x) {
                glm::dvec3 intersect, normal;
                Ray ray = _camera.getRay(x, y, w, h);

                _image->setPixel(x, y, {0, 0, 0});
                std::vector<Entity*> entities = _scene->intersect(ray);
                for (Entity* entity : entities) {
                    if (entity->intersect(ray, intersect, normal)) {
                        _image->setPixel(x, y, {0, 1, 0});
                    }
                }

                // TODO Implement this
            }
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
