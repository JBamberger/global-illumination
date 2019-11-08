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

    auto sphere = std::make_unique<ImplicitSphere>();
    sphere->radius = 1;
    scene.push_back(sphere.get());

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
