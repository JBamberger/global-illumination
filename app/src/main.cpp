#include <QApplication>

#include <memory>

#include <CheckerboardMaterial.h>
#include <camera.h>
#include <entities.h>
#include <gui.h>
#include <iostream>
#include <random>

constexpr glm::dvec3 black(0, 0, 0);
constexpr glm::dvec3 white(1, 1, 1);

constexpr glm::dvec3 red(1, 0, 0);
constexpr glm::dvec3 green(0, 1, 0);
constexpr glm::dvec3 blue(0, 0, 1);

constexpr glm::dvec3 yellow(1, 1, 0);
constexpr glm::dvec3 cyan(0, 1, 1);
constexpr glm::dvec3 magenta(1, 0, 1);

// dark blue glm::dvec3 { 0.3, 0.3, 1 }

std::vector<std::unique_ptr<Entity>> random_spheres(BoundingBox bounds, int count)
{
    std::array<glm::dvec3, 8> colors;
    colors[0] = black;
    colors[1] = white;
    colors[2] = red;
    colors[3] = green;
    colors[4] = blue;
    colors[5] = yellow;
    colors[6] = cyan;
    colors[7] = magenta;

    std::default_random_engine gen;
    std::normal_distribution<double> x_coord(0, (bounds.max.x - bounds.min.x) / 6);
    std::normal_distribution<double> y_coord(0, (bounds.max.y - bounds.min.y) / 6);
    std::normal_distribution<double> z_coord(0, (bounds.max.z - bounds.min.z) / 6);
    const std::uniform_int_distribution<int> color_dist(0, 7);
    // const std::uniform_real_distribution<double> x_coord(bounds.min.x, bounds.max.x);
    // const std::uniform_real_distribution<double> y_coord(bounds.min.y, bounds.max.y);
    // const std::uniform_real_distribution<double> z_coord(bounds.min.z, bounds.max.z);
    const std::uniform_real_distribution<double> rad(0.1, 1);

    std::vector<std::unique_ptr<Entity>> scene;
    scene.reserve(count);

    for (auto i = 0; i < count; ++i) {
        auto s1 = std::make_unique<ImplicitSphere>(
            glm::dvec3{x_coord(gen), y_coord(gen), z_coord(gen)}, rad(gen));
        s1->setMaterial(std::make_shared<SimpleMaterial>(colors[color_dist(gen)]));
        scene.push_back(std::move(s1));
    }

    return scene;
}

std::vector<std::unique_ptr<Entity>> create_sphere_scene()
{
    std::vector<std::unique_ptr<Entity>> scene;
    auto s1 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 2, 0}, 1.0);
    auto s2 = std::make_unique<ImplicitSphere>(glm::dvec3{0, -2, 0}, 1.0);
    auto s3 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 0, 2}, 1.0);
    auto s4 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 0, -2}, 1.0);
    s1->setMaterial(std::make_shared<SimpleMaterial>(blue));
    s2->setMaterial(std::make_shared<SimpleMaterial>(green));
    s3->setMaterial(std::make_shared<SimpleMaterial>(red));
    s4->setMaterial(std::make_shared<SimpleMaterial>(yellow));
    scene.push_back(std::move(s1));
    scene.push_back(std::move(s2));
    scene.push_back(std::move(s3));
    scene.push_back(std::move(s4));

    return scene;
}

std::vector<std::unique_ptr<Entity>> create_complex_scene()
{
    using namespace entities;

    std::vector<std::unique_ptr<Entity>> scene;

    // explicit sphere
    auto esphere = makeSphere(glm::dvec3{0, -2, 2}, 0.5, 1);
    esphere->setMaterial(std::make_shared<SimpleMaterial>(blue));
    esphere->material->ambient = 0.3;
    scene.push_back(std::move(esphere));

    // explicit cube
    auto cube = makeCube(glm::dvec3{0, -2, 0}, 0.5);
    cube->setMaterial(std::make_shared<SimpleMaterial>(red));
    // cube->material->diffuse = 0.0;
    // cube->material->refractive_index = 1.5;
    // cube->material->refractive = 0.7;
    scene.push_back(std::move(cube));

    // Defines x,y and z axis indicators
    const auto tip = glm::dvec3{0, 0, 0};
    const auto id_len = 2;
    auto x_axis = makeCone(tip + glm::dvec3{id_len, 0, 0}, tip, 0.1, 10);
    x_axis->setMaterial(std::make_shared<SimpleMaterial>(red));
    scene.push_back(std::move(x_axis));

    auto y_axis = makeCone(tip + glm::dvec3{0, id_len, 0}, tip, 0.1, 10);
    y_axis->setMaterial(std::make_shared<SimpleMaterial>(green));
    scene.push_back(std::move(y_axis));

    auto z_axis = makeCone(tip + glm::dvec3{0, 0, id_len}, tip, 0.1, 10);
    z_axis->setMaterial(std::make_shared<SimpleMaterial>(blue));
    scene.push_back(std::move(z_axis));

    auto sphere = std::make_unique<ImplicitSphere>(glm::dvec3{0, 0, 0}, 1.0);
    sphere->setMaterial(std::make_shared<CheckerboardMaterial>());
    sphere->material->ambient = 0.3;
    sphere->material->diffuse = 1.0;
    sphere->material->specular = 0.0;
    sphere->material->specular_exponent = 0;
    sphere->material->reflective = 0.0;
    sphere->material->refractive = 0.0;
    sphere->material->refractive_index = 1;
    scene.push_back(std::move(sphere));

    auto sphere2 = std::make_unique<ImplicitSphere>(glm::dvec3{1.5, 0.5, 0.5}, 0.4);
    sphere2->setMaterial(std::make_shared<SimpleMaterial>(blue));
    sphere2->material->ambient = 0.0;
    sphere2->material->diffuse = 0.0;
    sphere2->material->specular = 0.0;
    sphere2->material->specular_exponent = 0;
    sphere2->material->reflective = 0.0;
    sphere2->material->refractive = 1.0;
    sphere2->material->refractive_index = 1.5;
    scene.push_back(std::move(sphere2));

    // triangle upper right corner
    auto t3 = std::make_unique<ImplicitSphere>(glm::dvec3{1, 2.5, 0.5}, 0.5);
    t3->setMaterial(std::make_shared<SimpleMaterial>(magenta));
    scene.push_back(std::move(t3));

    auto t1 = std::make_unique<ImplicitSphere>(glm::dvec3{-1, 1.5, -0.5}, 0.5);
    t1->setMaterial(std::make_shared<SimpleMaterial>(cyan));
    scene.push_back(std::move(t1));

    auto t2 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 2, 0}, 0.5);
    t2->setMaterial(std::make_shared<SimpleMaterial>(yellow));
    scene.push_back(std::move(t2));

    // floor
    auto quad = makeQuad({10, 10, -1}, {-10, 10, -1}, {-10, -10, -1}, {10, -10, -1});
    quad->setMaterial(std::make_shared<CheckerboardMaterial>(10, green, magenta));
    // quad->setMaterial(std::make_shared<SimpleMaterial>(magenta);
    quad->material->reflective = 1.0;
    quad->material->ambient = 0.0;
    quad->material->diffuse = 0.3;
    quad->material->specular = 0.7;
    scene.push_back(std::move(quad));

    return scene;
}

std::vector<std::unique_ptr<Entity>> create_tex_mapping_scene()
{
    using namespace entities;

    std::vector<std::unique_ptr<Entity>> scene;

    // floor
    // auto quad = make_quad({0, -3, -3}, {0, 3, -3}, {0, 3, 3}, {0, -3, 3});
    auto quad = makeQuad({10, 10, -1}, {-10, 10, -1}, {-10, -10, -1}, {10, -10, -1});
    quad->setMaterial(std::make_shared<CheckerboardMaterial>(10, black, green));
    scene.push_back(std::move(quad));

    return scene;
}

std::vector<std::unique_ptr<Entity>> create_material_scene()
{
    std::vector<std::unique_ptr<Entity>> scene;

    const auto mk_sphere = [&scene](glm::dvec3 c, double r) {
        auto s1 = std::make_unique<ImplicitSphere>(c, 0.5);
        s1->setMaterial(std::make_shared<SimpleMaterial>(green));
        s1->material->reflective = 0.5;
        s1->material->diffuse = 0.5;
        s1->material->specular = 0;
        s1->material->rough_radius = r;
        s1->material->reflect_rays = 5;
        scene.push_back(std::move(s1));
    };

    mk_sphere(glm::dvec3{0, -2, 0}, 0.0);
    mk_sphere(glm::dvec3{0, -1, 0}, 0.1);
    mk_sphere(glm::dvec3{0, 0, 0}, 0.4);
    mk_sphere(glm::dvec3{0, 1, 0}, 0.7);
    mk_sphere(glm::dvec3{0, 2, 0}, 1.0);

    // floor
    auto quad = entities::makeQuad({10, 10, -1}, {-10, 10, -1}, {-10, -10, -1}, {10, -10, -1});
    quad->setMaterial(std::make_shared<CheckerboardMaterial>(10, green, magenta));
    // quad->setMaterial(std::make_shared<SimpleMaterial>(magenta);
    quad->material->reflective = 0.7;
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

    Octree scene({-20, -20, -20}, {20, 20, 20});

    // auto elems = create_sphere_scene();
    // auto elems = create_complex_scene();
    // auto elems = create_tex_mapping_scene();
    // auto elems = random_spheres(scene.bounds(), 100);
    auto elems = create_material_scene();
    for (const auto& entity : elems) scene.pushBack(entity.get());

    raytracer.set_scene(&scene);

    std::cout << scene << std::endl;

    Gui window(500, 500, raytracer);
    window.show();
    return QApplication::exec();
}
