#pragma once
#include "entity.h"

struct triangle final : entity {
    triangle(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C);

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;
    glm::dvec3 get_color_at_intersect(glm::dvec3 intersect) const override;

    inline glm::dvec3 normal() const { return glm::normalize(glm::cross(B - A, C - A)); }

    /// corner points (ccw)
    glm::dvec3 A = {0, 0, 0};
    glm::dvec3 B = {0, 0, 0};
    glm::dvec3 C = {0, 0, 0};

    // /// texture coordinates
    // glm::dvec2 t1 = {0, 0};
    // glm::dvec2 t2 = {0, 0};
    // glm::dvec2 t3 = {0, 0};
};