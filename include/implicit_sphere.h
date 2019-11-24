#pragma once
#include "entity.h"

struct implicit_sphere final : entity {
    implicit_sphere() : implicit_sphere({0, 0, 0}, 1) {}
    implicit_sphere(glm::dvec3 center, double radius);

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;
    glm::dvec3 get_color_at_intersect(glm::dvec3 intersect) const override;

    glm::dvec3 center = {0, 0, 0};
    double radius = 0;
};