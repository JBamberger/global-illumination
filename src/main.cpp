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
    octree scene({-20, -20, -20}, {20, 20, 20});

#if 0
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

#endif

    // explicit cone
    const auto tip = glm::dvec3{0, 0, 0};

    auto cone1 = std::make_unique<explicit_entity>(
        explicit_entity::make_cone(tip + glm::dvec3{2, 0, 0}, tip, 0.1, 10));
    auto cone2 = std::make_unique<explicit_entity>(
        explicit_entity::make_cone(tip + glm::dvec3{0, 2, 0}, tip, 0.1, 10));
    auto cone3 = std::make_unique<explicit_entity>(
        explicit_entity::make_cone(tip + glm::dvec3{0, 0, 2}, tip, 0.1, 10));
    cone1->material = red;
    cone2->material = green;
    cone3->material = blue;
    scene.push_back(cone1.get());
    scene.push_back(cone2.get());
    scene.push_back(cone3.get());

#if 0
    // sphere at center
    auto sphere = std::make_unique<implicit_sphere>(glm::dvec3{0, 0, 0}, 1.0);
    // sphere->radius = 1;
    sphere->material = green;
    sphere->material.specular_exponent = 64;
    scene.push_back(sphere.get());

    // triangle upper right corner
    auto t3 = std::make_unique<triangle>(glm::dvec3{1, 2.5, 0.5}, glm::dvec3{1, 2.5, 2.5},
                                         glm::dvec3{1, 0.5, 2.5});
    t3->material = red;
    scene.push_back(t3.get());

    auto t1 = std::make_unique<triangle>(glm::dvec3{-1, 1.5, -0.5}, glm::dvec3{-1, 1.5, 1.5},
                                         glm::dvec3{-1, -0.5, 1.5});
    t1->material = blue;
    scene.push_back(t1.get());

    auto t2 =
        std::make_unique<triangle>(glm::dvec3{0, 2, 0}, glm::dvec3{0, 2, 2}, glm::dvec3{0, 0, 2});
    t2->material = green;
    scene.push_back(t2.get());

    auto quad = std::make_unique<explicit_entity>(
        explicit_entity::make_quad(glm::dvec3{-10, -10, -1}, glm::dvec3{10, -10, -1},
                                   glm::dvec3{10, 10, -1}, glm::dvec3{-10, 10, -1}));
    quad->material = Material(glm::dvec3{0.5, 0.5, 0.5});
    quad->material.glazed = 1.0;
    quad->material.ambient = 0.0;
    quad->material.diffuse = 0.3;
    quad->material.specular = 0.7;
    scene.push_back(quad.get());

#endif

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
