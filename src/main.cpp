#include <QApplication>

#include <memory>

#include "camera.h"
#include "checkerboard_material.h"
#include "entities.h"
#include "gui.h"

constexpr glm::dvec3 black(0, 0, 0);
constexpr glm::dvec3 white(1, 1, 1);

constexpr glm::dvec3 red(1, 0, 0);
constexpr glm::dvec3 green(0, 1, 0);
constexpr glm::dvec3 blue(0, 0, 1);

constexpr glm::dvec3 yellow(1, 1, 0);
constexpr glm::dvec3 cyan(0, 1, 1);
constexpr glm::dvec3 magenta(1, 0, 1);

// dark blue glm::dvec3 { 0.3, 0.3, 1 }

std::vector<std::unique_ptr<entity>> create_sphere_scene()
{
    std::vector<std::unique_ptr<entity>> scene;
    auto s1 = std::make_unique<implicit_sphere>(glm::dvec3{0, 2, 0}, 1.0);
    auto s2 = std::make_unique<implicit_sphere>(glm::dvec3{0, -2, 0}, 1.0);
    auto s3 = std::make_unique<implicit_sphere>(glm::dvec3{0, 0, 2}, 1.0);
    auto s4 = std::make_unique<implicit_sphere>(glm::dvec3{0, 0, -2}, 1.0);
    s1->material = std::make_shared<simple_material>(blue);
    s2->material = std::make_shared<simple_material>(green);
    s3->material = std::make_shared<simple_material>(red);
    s4->material = std::make_shared<simple_material>(yellow);
    scene.push_back(std::move(s1));
    scene.push_back(std::move(s2));
    scene.push_back(std::move(s3));
    scene.push_back(std::move(s4));

    return scene;
}

std::vector<std::unique_ptr<entity>> create_complex_scene()
{
    using namespace entities;

    std::vector<std::unique_ptr<entity>> scene;

    // explicit sphere
    auto esphere = make_sphere(glm::dvec3{0, -2, 2}, 0.5, 1);
    esphere->material = std::make_shared<simple_material>(blue);
    esphere->material->ambient = 0.3;
    scene.push_back(std::move(esphere));

    // explicit cube
    auto cube = make_cube(glm::dvec3{0, -2, 0}, 0.5);
    cube->material = std::make_shared<simple_material>(red);
    scene.push_back(std::move(cube));

    // Defines x,y and z axis indicators
    const auto tip = glm::dvec3{0, 0, 0};
    const auto id_len = 2;
    auto x_axis = make_cone(tip + glm::dvec3{id_len, 0, 0}, tip, 0.1, 10);
    x_axis->material = std::make_shared<simple_material>(red);
    scene.push_back(std::move(x_axis));

    auto y_axis = make_cone(tip + glm::dvec3{0, id_len, 0}, tip, 0.1, 10);
    y_axis->material = std::make_shared<simple_material>(green);
    scene.push_back(std::move(y_axis));

    auto z_axis = make_cone(tip + glm::dvec3{0, 0, id_len}, tip, 0.1, 10);
    z_axis->material = std::make_shared<simple_material>(blue);
    scene.push_back(std::move(z_axis));

    auto sphere = std::make_unique<implicit_sphere>(glm::dvec3{0, 0, 0}, 1.0);
    sphere->material = std::make_shared<checkerboard_material>();
    sphere->material->ambient = 0.3;
    sphere->material->diffuse = 0.6;
    sphere->material->specular = 0.6;
    sphere->material->specular_exponent = 0;
    sphere->material->reflective = 0.4;
    sphere->material->refractive = 0.0;
    sphere->material->refractive_index = 1;
    // sphere->material = std::make_shared<simple_material>(glm::dvec3(0, 1, 0));
    // sphere->material->specular_exponent = 64;
    // sphere->material->ambient = 0;
    // sphere->material->diffuse = 1;
    // sphere->material->specular = 0.5;
    // sphere->material->reflective = 0;
    // sphere->material->refractive = 1;
    // sphere->material->refractive_index = 1;
    scene.push_back(std::move(sphere));

    // triangle upper right corner
    auto t3 = std::make_unique<triangle>(glm::dvec3{1, 2.5, 0.5}, glm::dvec3{1, 2.5, 2.5},
                                         glm::dvec3{1, 0.5, 2.5});
    t3->material = std::make_shared<simple_material>(magenta);
    scene.push_back(std::move(t3));

    auto t1 = std::make_unique<triangle>(glm::dvec3{-1, 1.5, -0.5}, glm::dvec3{-1, 1.5, 1.5},
                                         glm::dvec3{-1, -0.5, 1.5});
    t1->material = std::make_shared<simple_material>(cyan);
    scene.push_back(std::move(t1));

    auto t2 =
        std::make_unique<triangle>(glm::dvec3{0, 2, 0}, glm::dvec3{0, 2, 2}, glm::dvec3{0, 0, 2});
    t2->material = std::make_shared<simple_material>(yellow);
    scene.push_back(std::move(t2));

    // floor
    auto quad = make_quad({10, 10, -1}, {-10, 10, -1}, {-10, -10, -1}, {10, -10, -1});
    quad->material = std::make_shared<checkerboard_material>(10, glm::dvec3{0, 0, 0}, magenta);
    // quad->material = std::make_shared<simple_material>(glm::dvec3{0.5, 0.5, 0.5});
    quad->material->reflective = 1.0;
    quad->material->ambient = 0.0;
    quad->material->diffuse = 0.3;
    quad->material->specular = 0.7;
    scene.push_back(std::move(quad));

    return scene;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Camera camera(glm::dvec3{10, 0, 0});
    glm::dvec3 light{10, 10, 15};

    RayTracer raytracer(camera, light);

    auto mat_red = std::make_shared<simple_material>(red);
    auto mat_green = std::make_shared<simple_material>(green);
    auto mat_blue = std::make_shared<simple_material>(blue);

    // Set up scene
    octree scene({-20, -20, -20}, {20, 20, 20});

    // auto elems = create_sphere_scene();
    auto elems = create_complex_scene();
    for (const auto& entity : elems) scene.push_back(entity.get());

    raytracer.set_scene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
