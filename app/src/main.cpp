#include <QApplication>

#include <memory>

#include "Material.h"
#include <ObjReader.h>
#include <camera.h>
#include <entities.h>
#include <fstream>
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

std::vector<std::unique_ptr<Entity>> createCornell()
{
    using namespace entities;

    std::vector<std::unique_ptr<Entity>> scene;
    std::unique_ptr<Entity> face;

    const auto s = 3.0;

    glm::dvec3 p000 = {-s, -s, -s};
    glm::dvec3 p001 = {-s, -s, s};
    glm::dvec3 p010 = {-s, s, -s};
    glm::dvec3 p011 = {-s, s, s};
    glm::dvec3 p100 = {s, -s, -s};
    glm::dvec3 p101 = {s, -s, s};
    glm::dvec3 p110 = {s, s, -s};
    glm::dvec3 p111 = {s, s, s};

    const auto mat_red = std::make_shared<LambertianMaterial>(red);
    const auto mat_green = std::make_shared<LambertianMaterial>(green);
    const auto mat_white = std::make_shared<LambertianMaterial>(white);

    // left face
    face = makeQuad(p100, p000, p001, p101);
    face->setMaterial(mat_red);
    scene.push_back(std::move(face));
    // right face
    face = makeQuad(p110, p111, p011, p010);
    face->setMaterial(mat_green);
    scene.push_back(std::move(face));
    // top face
    face = makeQuad(p101, p001, p011, p111);
    face->setMaterial(mat_white);
    scene.push_back(std::move(face));
    // bottom face
    face = makeQuad(p110, p010, p000, p100);
    face->setMaterial(mat_white);
    scene.push_back(std::move(face));
    // back face
    face = makeQuad(p000, p010, p011, p001);
    face->setMaterial(mat_white);
    scene.push_back(std::move(face));

    face = makeCuboid(glm::dvec3(0, 0, 3), glm::dvec3(5, 5, 0.1));
    face->setMaterial(std::make_shared<DiffuseLight>(white));
    scene.push_back(std::move(face));

    face = std::make_unique<Sphere>(glm::dvec3{-1.5, 1.5, -2}, 1.0);
    face->setMaterial(std::make_shared<MetalLikeMaterial>(white, 0.0)); // 0.5
    scene.push_back(std::move(face));

    auto c = obj::makeCuboid({0, 0, 0}, {2, 2, 4});
    c = obj::rotate_z(std::move(c), -glm::pi<double>() / 10);
    c = obj::translate(std::move(c), {-1.5, -1.5, -1});
    face = std::make_unique<ExplicitEntity>(c);
    face->setMaterial(std::make_shared<LambertianMaterial>(white));
    scene.push_back(std::move(face));

    face = std::make_unique<Sphere>(glm::dvec3{1.5, -1.0, -2}, 1.0);
    face->setMaterial(std::make_shared<Dielectric>(1.4));
    scene.push_back(std::move(face));

    // Defines x,y and z axis indicators
    //        const auto tip = glm::dvec3{0, 0, 0};
    //        const auto id_len = 2;
    //        auto x_axis = makeCone(tip + glm::dvec3{id_len, 0, 0}, tip, 0.1, 10);
    //        x_axis->setMaterial(std::make_shared<LambertianMaterial>(red));
    //        scene.push_back(std::move(x_axis));
    //
    //        auto y_axis = makeCone(tip + glm::dvec3{0, id_len, 0}, tip, 0.1, 10);
    //        y_axis->setMaterial(std::make_shared<LambertianMaterial>(green));
    //        scene.push_back(std::move(y_axis));
    //
    //        auto z_axis = makeCone(tip + glm::dvec3{0, 0, id_len}, tip, 0.1, 10);
    //        z_axis->setMaterial(std::make_shared<LambertianMaterial>(blue));
    //        scene.push_back(std::move(z_axis));

    //    const auto load_pig_part = [](const std::string& file) {
    //        auto part = std::make_unique<ExplicitEntity>(std::vector<Triangle>());
    //        std::ifstream pigstream(file);
    //        if (pigstream.is_open()) {
    //            pigstream >> *part;
    //            pigstream.close();
    //        }
    //        part = translate(std::move(part), {1, -0.5, 2});
    //        part = rotate_x(std::move(part), -glm::pi<double>() / 2);
    //        part = rotate_z(std::move(part), -glm::pi<double>() / 3);
    //        part = scale(std::move(part), 3);
    //        part = translate(std::move(part), {0, 0, -1});
    //        return part;
    //    };
    //
    //    face = load_pig_part("D:/dev/global-illumination/share/pig_body.obj");
    //    face->setMaterial(std::make_shared<LambertianMaterial>(glm::dvec3(0.9, 0.6, 0.9)));
    //    scene.push_back(std::move(face));
    //
    //    face = load_pig_part("D:/dev/global-illumination/share/pig_eyes.obj");
    //    face->setMaterial(std::make_shared<MetalLikeMaterial>(white, 0.5));
    //    scene.push_back(std::move(face));
    //
    //    face = load_pig_part("D:/dev/global-illumination/share/pig_pupils.obj");
    //    face->setMaterial(std::make_shared<LambertianMaterial>(red));
    //    scene.push_back(std::move(face));
    //
    //    face = load_pig_part("D:/dev/global-illumination/share/pig_tongue.obj");
    //    face->setMaterial(std::make_shared<DiffuseLight>(0.5 * red));
    //    scene.push_back(std::move(face));

    return scene;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Camera camera(glm::dvec3{14, 0, 0});

    // scene setup
    const auto scene = std::make_shared<Octree>(glm::dvec3{-20, -20, -20}, glm::dvec3{20, 20, 20});
    auto elems = createCornell();
    for (const auto& entity : elems)
        scene->pushBack(entity.get());

    RayTracer raytracer(camera, scene);

    Gui window(500, 500, raytracer);
    window.show();
    return QApplication::exec();
}
