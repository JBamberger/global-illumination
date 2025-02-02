/**
 *    Copyright 2020 Jannik Bamberger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Scene.h"

Scene::Scene(std::filesystem::path shareDir, glm::dvec3 min, glm::dvec3 max)
    : share_dir_(std::move(shareDir)), tree_(std::make_shared<Octree>(min, max))
{
}

void Scene::useSceneSetting(SceneSetting setting)
{
    clear();
    addCornellBox();
    switch (setting) {
    case SceneSetting::Cornell:
        addCornellContent();
        break;
    case SceneSetting::Exam:
        addExamRender();
        break;
    case SceneSetting::Pig:
        addPig();
        break;
    case SceneSetting::Cow:
        addCow();
        break;
    case SceneSetting::Dragon:
        addDragon();
        break;
    default:
        return;
    }
}

Scene& Scene::addCornellBox(const double side_len,
                            const double light_size,
                            const double light_intensity)
{
    std::unique_ptr<Entity> face;

    const auto half_side = side_len / 2.0;

    glm::dvec3 p000 = {-half_side, -half_side, -half_side};
    glm::dvec3 p001 = {-half_side, -half_side, half_side};
    glm::dvec3 p010 = {-half_side, half_side, -half_side};
    glm::dvec3 p011 = {-half_side, half_side, half_side};
    glm::dvec3 p100 = {half_side, -half_side, -half_side};
    glm::dvec3 p101 = {half_side, -half_side, half_side};
    glm::dvec3 p110 = {half_side, half_side, -half_side};
    glm::dvec3 p111 = {half_side, half_side, half_side};

    const auto mat_red = std::make_shared<LambertianMaterial>(red);
    const auto mat_green = std::make_shared<LambertianMaterial>(green);
    const auto mat_white = std::make_shared<LambertianMaterial>(white);

    // left face
    face = entities::makeQuad(p100, p000, p001, p101);
    face->setMaterial(mat_red);
    insert(std::move(face));
    // right face
    face = entities::makeQuad(p110, p111, p011, p010);
    face->setMaterial(mat_green);
    insert(std::move(face));
    // top face
    face = entities::makeQuad(p101, p001, p011, p111);
    face->setMaterial(mat_white);
    insert(std::move(face));
    // bottom face
    face = entities::makeQuad(p110, p010, p000, p100);
    face->setMaterial(mat_white);
    insert(std::move(face));
    // back face
    face = entities::makeQuad(p000, p010, p011, p001);
    face->setMaterial(mat_white);
    insert(std::move(face));

    //    face = entities::makeCuboid(glm::dvec3(0, 0, 3), glm::dvec3(light_size, light_size, 0.1));
    const auto half_light = light_size / 2.0;
    const auto light_height = 2.999999;
    glm::dvec3 a(-half_light, -half_light, light_height);
    glm::dvec3 b(-half_light, half_light, light_height);
    glm::dvec3 c(half_light, half_light, light_height);
    glm::dvec3 d(half_light, -half_light, light_height);
    face = entities::makeQuad(a, b, c, d);
    face->setMaterial(std::make_shared<DiffuseLight>(light_intensity * white));
    insert(std::move(face));

    return *this;
}

Scene& Scene::addCornellContent()
{
    std::unique_ptr<Entity> face;

    face = std::make_unique<Sphere>(glm::dvec3{-1.5, 1.5, -2}, 1.0);
    face->setMaterial(std::make_shared<MetalLikeMaterial>(white, 0.0)); // 0.5
    insert(std::move(face));

    face = obj::Transform()
               .rotate_z(-glm::pi<double>() / 10)
               .translate({-1.5, -1.5, -1})
               .to_bvh(obj::makeCuboid({0, 0, 0}, {2, 2, 4}));
    face->setMaterial(std::make_shared<LambertianMaterial>(white));
    insert(std::move(face));

    //    face = obj::Transform()
    //               .rotate_z(glm::pi<double>() / 10)
    //               .translate({2, 2, -2.5})
    //               .to_bvh(obj::makeCuboid({0, 0, 0}, {1, 1, 1}));
    //    face->setMaterial(std::make_shared<DiffuseLight>(7.0 * glm::dvec3(1,1,1)));
    //    insert(std::move(face));

    face = std::make_unique<Sphere>(glm::dvec3{1.5, 0.0, -2}, 1.0);
    face->setMaterial(std::make_shared<Dielectric>(1.4));
    insert(std::move(face));

    return *this;
}

Scene& Scene::addExamRender()
{

    std::unique_ptr<Entity> face;

    face = std::make_unique<Sphere>(glm::dvec3{-1.5, 1.5, -2}, 1.0);
    face->setMaterial(std::make_shared<MetalLikeMaterial>(white, 0.0)); // 0.5
    insert(std::move(face));

    face = obj::Transform()
               .rotate_z(-glm::pi<double>() / 10)
               .translate({-1.5, -1.5, -1})
               .to_bvh(obj::makeCuboid({0, 0, 0}, {2, 2, 4}));
    face->setMaterial(std::make_shared<LambertianMaterial>(white));
    insert(std::move(face));

    //    face = obj::Transform()
    //               .rotate_z(glm::pi<double>() / 10)
    //               .translate({2, 2, -2.5})
    //               .to_bvh(obj::makeCuboid({0, 0, 0}, {1, 1, 1}));
    //    face->setMaterial(std::make_shared<DiffuseLight>(7.0 * glm::dvec3(1,1,1)));
    //    insert(std::move(face));

    face = std::make_unique<Sphere>(glm::dvec3{1.5, -2.0, -2.5}, 0.5);
    face->setMaterial(std::make_shared<Dielectric>(1.4));
    insert(std::move(face));

    const auto cow_tex = std::make_shared<ImageBackedTexture>(resolveFile(cow_tex_).string());
    face = obj::Transform()
               .center()
               .rotate_x(-glm::pi<double>() / 2)
               .rotate_z(glm::pi<double>() / 4)
               //        .scale(1)
               .translate({0.3, 0.3, -2.2})
               .to_bvh(resolveFile(cow_obj_).string());

    face->setMaterial(std::make_shared<LambertianMaterial>(cow_tex));
    insert(std::move(face));

    return addPig({-glm::pi<double>() / 2, 0.0, glm::pi<double>() / 4}, 1, {2, 2, -2.5}, false);
}

Scene& Scene::addAxisIndicator()
{
    std::unique_ptr<Entity> face;
    // Defines x,y and z axis indicators
    const auto tip = glm::dvec3{0, 0, 0};
    const auto id_len = 2;
    auto x_axis = entities::makeCone(tip + glm::dvec3{id_len, 0, 0}, tip, 0.1, 10);
    x_axis->setMaterial(std::make_shared<LambertianMaterial>(red));
    insert(std::move(x_axis));

    auto y_axis = entities::makeCone(tip + glm::dvec3{0, id_len, 0}, tip, 0.1, 10);
    y_axis->setMaterial(std::make_shared<LambertianMaterial>(green));
    insert(std::move(y_axis));

    auto z_axis = entities::makeCone(tip + glm::dvec3{0, 0, id_len}, tip, 0.1, 10);
    z_axis->setMaterial(std::make_shared<LambertianMaterial>(blue));
    insert(std::move(z_axis));

    return *this;
}

Scene& Scene::addPig(glm::dvec3 rotate, double scale, glm::dvec3 translation, bool add_box)
{
    std::unique_ptr<Entity> face;
    const auto load_pig_part = [&rotate, &scale, &translation](const std::string& file) {
        return obj::Transform()
            .translate({1, -0.5, 2})
            .rotate_x(rotate.x)
            .rotate_y(rotate.y)
            .rotate_z(rotate.z)
            .scale(scale)
            .translate(translation)
            .to_bvh(file);
    };

    face = load_pig_part(resolveFile(pig_body_obj_).string());
    face->setMaterial(std::make_shared<LambertianMaterial>(glm::dvec3(0.9, 0.6, 0.9)));
    insert(std::move(face));

    face = load_pig_part(resolveFile(pig_eyes_obj_).string());
    face->setMaterial(std::make_shared<LambertianMaterial>(white));
    insert(std::move(face));

    face = load_pig_part(resolveFile(pig_pupils_obj_).string());
    face->setMaterial(std::make_shared<LambertianMaterial>(black));
    insert(std::move(face));

    face = load_pig_part(resolveFile(pig_tongue_obj_).string());
    face->setMaterial(std::make_shared<DiffuseLight>(0.5 * red));
    insert(std::move(face));

    if (add_box) {
        face = obj::Transform()
                   .rotate_z(-glm::pi<double>() / 10)
                   .translate({0, 0, -2.75})
                   .to_bvh(obj::makeCuboid({0, 0, 0}, {4, 4, 0.5}));
        face->setMaterial(std::make_shared<LambertianMaterial>(white));
        insert(std::move(face));
    }

    return *this;
}

Scene& Scene::addDragon()
{
    auto face = obj::Transform()
                    .center()
                    .rotate_x(-glm::pi<double>() / 2)
                    .rotate_z(-2 * glm::pi<double>() / 3)
                    .scale(25)
                    .translate({0, 0, -1.6})
                    .to_bvh(resolveFile(dragon_obj_).string());
    //        face->setMaterial(std::make_shared<LambertianMaterial>(white));
    face->setMaterial(std::make_shared<Dielectric>(1.4));
    insert(std::move(face));

    return *this;
}

Scene& Scene::addCow()
{
    const auto cow_tex = std::make_shared<ImageBackedTexture>(resolveFile(cow_tex_).string());
    auto face = obj::Transform()
                    .center()
                    .rotate_x(-glm::pi<double>() / 2)
                    .rotate_z(glm::pi<double>() / 4)
                    .scale(2)
                    .translate({0, 0, -1.4})
                    .to_bvh(resolveFile(cow_obj_).string());

    face->setMaterial(std::make_shared<LambertianMaterial>(cow_tex));
    insert(std::move(face));

    return *this;
}

void Scene::clear()
{
    entities_.clear();
    tree_->clear();
}

std::shared_ptr<Octree> Scene::getTree() { return tree_; }

std::filesystem::path Scene::resolveFile(const std::string& relative_name) const
{
    std::filesystem::path fullname = share_dir_ / relative_name;
    if (!std::filesystem::exists(fullname) || !std::filesystem::is_regular_file(fullname)) {
        std::cerr << "Could not load file " << fullname << std::endl;
        exit(EXIT_FAILURE);
    }
    return fullname;
}

void Scene::insert(std::unique_ptr<Entity> entity)
{
    tree_->insert(entity.get());
    entities_.push_back(std::move(entity));
}
