#include <QApplication>

#include <memory>

#include "camera.h"
#include "entities.h"
#include "gui.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Camera camera(glm::dvec3{10, 0, 0});
    glm::dvec3 light{10, 10, 15};

    RayTracer raytracer(camera, light);

    Material red{{1, 0, 0}};
    Material green{{0, 1, 0}};
    Material blue{{0, 0, 1}};

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});

    // explicit sphere
    auto esphere = std::make_unique<explicit_entity>(
        explicit_entity::make_sphere(glm::dvec3{0, -2, 2}, 0.5, 0));
    esphere->material = Material{{0.3, 0.3, 1}};
    esphere->material.ambient = 0.3;
    scene.push_back(esphere.get());

    // explicit cube
    auto cube =
        std::make_unique<explicit_entity>(explicit_entity::make_cube(glm::dvec3{0, -2, 0}, 0.5));
    cube->material = red;
    scene.push_back(cube.get());

    // sphere at center
    auto sphere = std::make_unique<implicit_sphere>(glm::dvec3{0, 0, 0}, 1.0);
    // sphere->radius = 1;
    sphere->material = green;
    sphere->material.specular_exponent = 64;
    scene.push_back(sphere.get());

    // triangle upper right corner
    auto t3 = std::make_unique<triangle>();
    t3->A = {1, 2.5, 0.5};
    t3->B = {1, 2.5, 2.5};
    t3->C = {1, 0.5, 2.5};
    t3->material = red;
    scene.push_back(t3.get());

    auto t1 = std::make_unique<triangle>();
    t1->A = {-1, 1.5, -0.5};
    t1->B = {-1, 1.5, 1.5};
    t1->C = {-1, -0.5, 1.5};
    t1->material = blue;
    scene.push_back(t1.get());

    auto t2 = std::make_unique<triangle>();
    t2->A = {0, 2, 0};
    t2->B = {0, 2, 2};
    t2->C = {0, 0, 2};
    t2->material = green;
    scene.push_back(t2.get());

    auto quad = std::make_unique<explicit_entity>(
        explicit_entity::make_quad(glm::dvec3{-10, -10, -1}, glm::dvec3{10, -10, -1},
                                   glm::dvec3{10, 10, -1}, glm::dvec3{-10, 10, -1}));
    quad->material = Material(glm::dvec3{0.5, 0.5, 0.5});
    scene.push_back(quad.get());

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
