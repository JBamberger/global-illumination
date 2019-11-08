#include <QApplication>

#include <memory>

#include "camera.h"
#include "entities.h"
#include "gui.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Camera camera(glm::dvec3{10, 0, 0});
    glm::dvec3 light{10, 10, 10};

    RayTracer raytracer(camera, light);

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});
    // TODO Add objects to the scene
    // scene.push_back(...);

    // sphere at center
    auto sphere = std::make_unique<ImplicitSphere>();
    sphere->radius = 1;
    scene.push_back(sphere.get());

    // triangle upper right corner

    auto ur_triangle = std::make_unique<Triangle>();
    ur_triangle->A = {0, 2, 0};
    ur_triangle->B = {0, 2, 2};
    ur_triangle->C = {0, 0, 2};
    scene.push_back(ur_triangle.get());

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
