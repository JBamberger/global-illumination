#include <QApplication>

#include <memory>

#include "camera.h"
#include "checkerboard_material.h"
#include "entities.h"
#include "gui.h"

#define SHOW_AXIS
#define SHOW_CUBE
#define SHOW_EXPLICIT_SPHERE
#define SHOW_SPHERE
#define SHOW_TRIANGLES
#define SHOW_FLOOR

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Camera camera(glm::dvec3{10, 0, 0});
    glm::dvec3 light{10, 10, 15};

    RayTracer raytracer(camera, light);

    auto red = std::make_shared<simple_material>(glm::dvec3{1, 0, 0});
    auto green = std::make_shared<simple_material>(glm::dvec3{0, 1, 0});
    auto blue = std::make_shared<simple_material>(glm::dvec3{0, 0, 1});

    // Set up scene
    octree scene({-20, -20, -20}, {20, 20, 20});

#ifdef SHOW_EXPLICIT_SPHERE
    // explicit sphere
    auto esphere = std::make_unique<explicit_entity>(
        explicit_entity::make_sphere(glm::dvec3{0, -2, 2}, 0.5, 1));
    esphere->material = std::make_shared<simple_material>(glm::dvec3{0.3, 0.3, 1});
    esphere->material->ambient = 0.3;
    scene.push_back(esphere.get());
#endif

#ifdef SHOW_CUBE
    // explicit cube
    auto cube =
        std::make_unique<explicit_entity>(explicit_entity::make_cube(glm::dvec3{0, -2, 0}, 0.5));
    cube->material = red;
    scene.push_back(cube.get());

#endif

#ifdef SHOW_AXIS
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
#endif

#ifdef SHOW_SPHERE
    auto sphere = std::make_unique<implicit_sphere>(glm::dvec3{0, 0, 0}, 1.0);
    sphere->material = std::make_shared<checkerboard_material>();
    sphere->material->specular_exponent = 0;
    sphere->material->ambient = 0.0;
    sphere->material->diffuse = 1.0;
    sphere->material->specular = 0.0;
    sphere->material->glazed = 0.0;
    // sphere->material = std::make_shared<simple_material>(glm::dvec3(0, 1, 0));
    // sphere->material->specular_exponent = 64;
    // sphere->material->ambient = 0;
    // sphere->material->diffuse = 1;
    // sphere->material->specular = 0.5;
    // sphere->material->glazed = 0;
    // sphere->material->refractive = 1;
    // sphere->material->refractive_index = 1;
    scene.push_back(sphere.get());

    // auto sphere2 = std::make_unique<implicit_sphere>(glm::dvec3{-1, 1, 0}, 1.0);
    // sphere2->material = green;
    // scene.push_back(sphere2.get());
#endif

#ifdef SHOW_TRIANGLES
    // triangle upper right corner
    auto t3 = std::make_unique<triangle>(glm::dvec3{1, 2.5, 0.5}, glm::dvec3{1, 2.5, 2.5},
                                         glm::dvec3{1, 0.5, 2.5});
    t3->material = std::make_shared<simple_material>(glm::dvec3(1, 0, 0));
    t3->material->refractive = 1.0;
    t3->material->refractive_index = 2;
    scene.push_back(t3.get());

    auto t1 = std::make_unique<triangle>(glm::dvec3{-1, 1.5, -0.5}, glm::dvec3{-1, 1.5, 1.5},
                                         glm::dvec3{-1, -0.5, 1.5});
    t1->material = blue;
    scene.push_back(t1.get());

    auto t2 =
        std::make_unique<triangle>(glm::dvec3{0, 2, 0}, glm::dvec3{0, 2, 2}, glm::dvec3{0, 0, 2});
    t2->material = green;
    scene.push_back(t2.get());
#endif

#ifdef SHOW_FLOOR
    auto quad = std::make_unique<explicit_entity>(
        explicit_entity::make_quad(glm::dvec3{10, 10, -1}, glm::dvec3{-10, 10, -1},
                                   glm::dvec3{-10, -10, -1}, glm::dvec3{10, -10, -1}));
    quad->material =
        std::make_shared<checkerboard_material>(10, glm::dvec3{0, 0, 0}, glm::dvec3{0, 1, 0});

    // quad->material = std::make_shared<simple_material>(glm::dvec3{0.5, 0.5, 0.5});
    // quad->material->glazed = 1.0;
    // quad->material->ambient = 0.0;
    // quad->material->diffuse = 0.3;
    // quad->material->specular = 0.7;
    scene.push_back(quad.get());

#endif

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
