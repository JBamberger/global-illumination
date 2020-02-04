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
