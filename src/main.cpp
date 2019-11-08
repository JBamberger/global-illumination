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

    Material red{{1, 0, 0}};
    Material green{{0, 1, 0}};
    Material blue{{0, 0, 1}};

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});

    // sphere at center
    auto sphere = std::make_unique<ImplicitSphere>();
    sphere->radius = 1;
    sphere->material = green;
    scene.push_back(sphere.get());

    // triangle upper right corner
    auto t3 = std::make_unique<Triangle>();
    t3->A = {1, 2.5, 0.5};
    t3->B = {1, 2.5, 2.5};
    t3->C = {1, 0.5, 2.5};
    t3->material = red;
    scene.push_back(t3.get());

    auto t1 = std::make_unique<Triangle>();
    t1->A = {-1, 1.5, -0.5};
    t1->B = {-1, 1.5, 1.5};
    t1->C = {-1, -0.5, 1.5};
    t1->material = blue;
    scene.push_back(t1.get());

    auto t2 = std::make_unique<Triangle>();
    t2->A = {0, 2, 0};
    t2->B = {0, 2, 2};
    t2->C = {0, 0, 2};
    t2->material = green;
    scene.push_back(t2.get());

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
