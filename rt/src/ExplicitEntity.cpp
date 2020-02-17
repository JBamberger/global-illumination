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

#include "ExplicitEntity.h"

#include "ObjReader.h"
#include <ostream>
#include <string>

ExplicitEntity::ExplicitEntity(std::vector<Triangle> faces)
    : faces_(std::move(faces)), bbox_(obj::computeBBox(this->faces_))
{
}

void ExplicitEntity::setMaterial(std::shared_ptr<Material> material)
{
    this->material_ = material;
    for (auto& face : faces_) {
        face.setMaterial(material);
    }
}

bool ExplicitEntity::intersect(const Ray& ray, Hit& hit) const
{
    // TODO: check intersection direction (for triangles with only one side)
    // glm::dot(ray.direction, triangle.normal) > 0

    // quickly discard all rays that don't even intersect the bounding box
    if (!boundingBox().intersect(ray)) {
        return false;
    }

    auto min = std::numeric_limits<double>::infinity();
    Hit tmp_hit;
    for (const auto& t : faces_) {
        if (t.intersect(ray, tmp_hit)) {
            const auto d = glm::distance(tmp_hit.pos, ray.origin);
            if (d < min) {
                min = d;
                hit = tmp_hit;
            }
        }
    }
    return min != std::numeric_limits<double>::infinity();
}

BoundingBox ExplicitEntity::boundingBox() const { return bbox_; }
