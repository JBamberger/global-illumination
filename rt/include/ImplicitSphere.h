#pragma once
#include <Entity.h>

struct ImplicitSphere final : Entity {
    ImplicitSphere() : ImplicitSphere({0, 0, 0}, 1) {}
    ImplicitSphere(glm::dvec3 center, double radius);

    const Entity* intersect(const Ray& ray,
                            glm::dvec3& intersect,
                            glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;
    glm::dvec3 getColorAtIntersect(glm::dvec3 intersect) const override;

    glm::dvec3 center = {0, 0, 0};
    double radius = 0;
};