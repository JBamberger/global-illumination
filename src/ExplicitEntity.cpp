#include "entities.h"

bool ExplicitEntity::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const {
    // TODO: check intersection direction
    for (const auto& t : faces) {
        if (t.intersect(ray, intersect, normal)) {
            return true;
        }
    }
    return false;
}

BoundingBox ExplicitEntity::boundingBox() const {
    if (faces.empty()) {
        return BoundingBox{{0, 0, 0}, {0, 0, 0}};
    }
    const auto b1 = faces[0].boundingBox();
    glm::dvec3 min = b1.min;
    glm::dvec3 max = b1.max;

    for (const auto& t : faces) {
        min = glm::min(min, t.A);
        max = glm::max(max, t.A);
        min = glm::min(min, t.B);
        max = glm::max(max, t.B);
        min = glm::min(min, t.C);
        max = glm::max(max, t.C);
    }

    return BoundingBox{min, max};
}