#include <QApplication>

#include "Material.h"
#include <BVH.h>
#include <ObjReader.h>
#include <QCommandLineParser>
#include <Scene.h>
#include <camera.h>
#include <entities.h>
#include <filesystem>
#include <fstream>
#include <gui.h>
#include <iostream>
#include <memory>
#include <random>

// dark blue glm::dvec3 { 0.3, 0.3, 1 }

// std::vector<std::unique_ptr<Entity>> random_spheres(BoundingBox bounds, int count)
//{
//    std::array<glm::dvec3, 8> colors;
//    colors[0] = black;
//    colors[1] = white;
//    colors[2] = red;
//    colors[3] = green;
//    colors[4] = blue;
//    colors[5] = yellow;
//    colors[6] = cyan;
//    colors[7] = magenta;
//
//    std::default_random_engine gen;
//    std::normal_distribution<double> x_coord(0, (bounds.max.x - bounds.min.x) / 6);
//    std::normal_distribution<double> y_coord(0, (bounds.max.y - bounds.min.y) / 6);
//    std::normal_distribution<double> z_coord(0, (bounds.max.z - bounds.min.z) / 6);
//    const std::uniform_int_distribution<int> color_dist(0, 7);
//    // const std::uniform_real_distribution<double> x_coord(bounds.min.x, bounds.max.x);
//    // const std::uniform_real_distribution<double> y_coord(bounds.min.y, bounds.max.y);
//    // const std::uniform_real_distribution<double> z_coord(bounds.min.z, bounds.max.z);
//    const std::uniform_real_distribution<double> rad(0.1, 1);
//
//    std::vector<std::unique_ptr<Entity>> scene;
//    scene.reserve(count);
//
//    for (auto i = 0; i < count; ++i) {
//        auto s1 = std::make_unique<ImplicitSphere>(
//            glm::dvec3{x_coord(gen), y_coord(gen), z_coord(gen)}, rad(gen));
//        s1->setMaterial(std::make_shared<SimpleMaterial>(colors[color_dist(gen)]));
//        scene.push_back(std::move(s1));
//    }
//
//    return scene;
//}
//
// std::vector<std::unique_ptr<Entity>> create_sphere_scene()
//{
//    std::vector<std::unique_ptr<Entity>> scene;
//    auto s1 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 2, 0}, 1.0);
//    auto s2 = std::make_unique<ImplicitSphere>(glm::dvec3{0, -2, 0}, 1.0);
//    auto s3 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 0, 2}, 1.0);
//    auto s4 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 0, -2}, 1.0);
//    s1->setMaterial(std::make_shared<SimpleMaterial>(blue));
//    s2->setMaterial(std::make_shared<SimpleMaterial>(green));
//    s3->setMaterial(std::make_shared<SimpleMaterial>(red));
//    s4->setMaterial(std::make_shared<SimpleMaterial>(yellow));
//    scene.push_back(std::move(s1));
//    scene.push_back(std::move(s2));
//    scene.push_back(std::move(s3));
//    scene.push_back(std::move(s4));
//
//    return scene;
//}
//
// std::vector<std::unique_ptr<Entity>> create_complex_scene()
//{
//    using namespace entities;
//
//    std::vector<std::unique_ptr<Entity>> scene;
//
//    // explicit sphere
//    auto esphere = makeSphere(glm::dvec3{0, -2, 2}, 0.5, 1);
//    esphere->setMaterial(std::make_shared<SimpleMaterial>(blue));
//    esphere->material_->ambient = 0.3;
//    scene.push_back(std::move(esphere));
//
//    // explicit cube
//    auto cube = makeCube(glm::dvec3{0, -2, 0}, 0.5);
//    cube->setMaterial(std::make_shared<SimpleMaterial>(red));
//    // cube->material->diffuse = 0.0;
//    // cube->material->refractive_index = 1.5;
//    // cube->material->refractive = 0.7;
//    scene.push_back(std::move(cube));
//
//    // Defines x,y and z axis indicators
//    const auto tip = glm::dvec3{0, 0, 0};
//    const auto id_len = 2;
//    auto x_axis = makeCone(tip + glm::dvec3{id_len, 0, 0}, tip, 0.1, 10);
//    x_axis->setMaterial(std::make_shared<SimpleMaterial>(red));
//    scene.push_back(std::move(x_axis));
//
//    auto y_axis = makeCone(tip + glm::dvec3{0, id_len, 0}, tip, 0.1, 10);
//    y_axis->setMaterial(std::make_shared<SimpleMaterial>(green));
//    scene.push_back(std::move(y_axis));
//
//    auto z_axis = makeCone(tip + glm::dvec3{0, 0, id_len}, tip, 0.1, 10);
//    z_axis->setMaterial(std::make_shared<SimpleMaterial>(blue));
//    scene.push_back(std::move(z_axis));
//
//    auto sphere = std::make_unique<ImplicitSphere>(glm::dvec3{0, 0, 0}, 1.0);
//    sphere->setMaterial(std::make_shared<CheckerboardMaterial>());
//    sphere->material_->ambient = 0.3;
//    sphere->material_->diffuse = 1.0;
//    sphere->material_->specular = 0.0;
//    sphere->material_->specular_exponent = 0;
//    sphere->material_->reflective = 0.0;
//    sphere->material_->refractive = 0.0;
//    sphere->material_->refractive_index = 1;
//    scene.push_back(std::move(sphere));
//
//    auto sphere2 = std::make_unique<ImplicitSphere>(glm::dvec3{1.5, 0.5, 0.5}, 0.4);
//    sphere2->setMaterial(std::make_shared<SimpleMaterial>(blue));
//    sphere2->material_->ambient = 0.0;
//    sphere2->material_->diffuse = 0.0;
//    sphere2->material_->specular = 0.0;
//    sphere2->material_->specular_exponent = 0;
//    sphere2->material_->reflective = 0.0;
//    sphere2->material_->refractive = 1.0;
//    sphere2->material_->refractive_index = 1.5;
//    scene.push_back(std::move(sphere2));
//
//    // triangle upper right corner
//    auto t3 = std::make_unique<ImplicitSphere>(glm::dvec3{1, 2.5, 0.5}, 0.5);
//    t3->setMaterial(std::make_shared<SimpleMaterial>(magenta));
//    scene.push_back(std::move(t3));
//
//    auto t1 = std::make_unique<ImplicitSphere>(glm::dvec3{-1, 1.5, -0.5}, 0.5);
//    t1->setMaterial(std::make_shared<SimpleMaterial>(cyan));
//    scene.push_back(std::move(t1));
//
//    auto t2 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 2, 0}, 0.5);
//    t2->setMaterial(std::make_shared<SimpleMaterial>(yellow));
//    scene.push_back(std::move(t2));
//
//    // floor
//    auto quad = makeQuad({10, 10, -1}, {-10, 10, -1}, {-10, -10, -1}, {10, -10, -1});
//    quad->setMaterial(std::make_shared<CheckerboardMaterial>(10, green, magenta));
//    // quad->setMaterial(std::make_shared<SimpleMaterial>(magenta);
//    quad->material_->reflective = 1.0;
//    quad->material_->ambient = 0.0;
//    quad->material_->diffuse = 0.3;
//    quad->material_->specular = 0.7;
//    scene.push_back(std::move(quad));
//
//    auto s1 = std::make_unique<ImplicitSphere>(glm::dvec3{0, 0, 2}, 0.5);
//    s1->setMaterial(std::make_shared<SimpleMaterial>(green));
//    s1->material_->reflective = 0.5;
//    s1->material_->diffuse = 0.5;
//    s1->material_->specular = 0;
//    s1->material_->rough_radius = 0.7;
//    s1->material_->reflect_rays = 5;
//    scene.push_back(std::move(s1));
//
//    return scene;
//}
//
// std::vector<std::unique_ptr<Entity>> create_tex_mapping_scene()
//{
//    using namespace entities;
//
//    std::vector<std::unique_ptr<Entity>> scene;
//
//    // floor
//    // auto quad = make_quad({0, -3, -3}, {0, 3, -3}, {0, 3, 3}, {0, -3, 3});
//    auto quad = makeQuad({10, 10, -1}, {-10, 10, -1}, {-10, -10, -1}, {10, -10, -1});
//    quad->setMaterial(std::make_shared<CheckerboardMaterial>(10, black, green));
//    scene.push_back(std::move(quad));
//
//    return scene;
//}
//
// std::vector<std::unique_ptr<Entity>> create_material_scene()
//{
//    std::vector<std::unique_ptr<Entity>> scene;
//
//    const auto mk_sphere = [&scene](glm::dvec3 c, double r) {
//        auto s1 = std::make_unique<ImplicitSphere>(c, 0.5);
//        s1->setMaterial(std::make_shared<SimpleMaterial>(green));
//        s1->material_->reflective = 0.5;
//        s1->material_->diffuse = 0.5;
//        s1->material_->specular = 0;
//        s1->material_->rough_radius = r;
//        s1->material_->reflect_rays = 5;
//        scene.push_back(std::move(s1));
//    };
//
//    mk_sphere(glm::dvec3{0, -2, 0}, 0.0);
//    mk_sphere(glm::dvec3{0, -1, 0}, 0.1);
//    mk_sphere(glm::dvec3{0, 0, 0}, 0.4);
//    mk_sphere(glm::dvec3{0, 1, 0}, 0.7);
//    mk_sphere(glm::dvec3{0, 2, 0}, 1.0);
//
//    // floor
//    auto quad = entities::makeQuad({10, 10, -1}, {-10, 10, -1}, {-10, -10, -1}, {10, -10, -1});
//    quad->setMaterial(std::make_shared<CheckerboardMaterial>(10, green, magenta));
//    // quad->setMaterial(std::make_shared<SimpleMaterial>(magenta);
//    quad->material_->reflective = 0.7;
//    quad->material_->ambient = 0.0;
//    quad->material_->diffuse = 0.3;
//    quad->material_->specular = 0.7;
//    scene.push_back(std::move(quad));
//
//    return scene;
//}

constexpr const char* app_name = "PathTracer";
constexpr const char* app_version = "v1.0.0";
constexpr const char* app_description = "PathTracer written for Global Illumination Methods WS1920";

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(app_name);
    QApplication::setApplicationVersion(app_version);

    // QCommandLineParser to read cli args
    QCommandLineParser parser;
    parser.setApplicationDescription(app_description);
    const auto help_option = parser.addHelpOption();
    const auto version_option = parser.addVersionOption();
    parser.addPositionalArgument("share_dir",
                                 "Directory containing the share files (objects, textures, ...).");
    parser.process(app);

    if (parser.isSet(help_option)) {
        parser.showHelp();
    }
    if (parser.isSet(version_option)) {
        parser.showVersion();
    }

    std::filesystem::path share_dir = "./share";
    const auto pa = parser.positionalArguments();
    if (!pa.isEmpty()) {
        share_dir = parser.positionalArguments()[0].toStdString();
    }
    if (!std::filesystem::exists(share_dir)) {
        std::cerr << "Share directory does not exist." << std::endl;
        parser.showHelp(EXIT_FAILURE);
    }

    std::cout << "ShareDir: " << share_dir << std::endl;

    Camera camera(glm::dvec3{14, 0, 0});

    // scene setup
    auto scene = Scene(share_dir, {-20, -20, -20}, {20, 20, 20});
    scene.addCornellBox().addCow();

    RayTracer raytracer(camera, scene.getTree());

    Gui window(500, 500, raytracer);
    window.show();
    return QApplication::exec();
}
