#pragma once
#include "Entity.h"

struct Triangle final : Entity {
    Triangle(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C);

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;
    glm::dvec3 getColorAtIntersect(glm::dvec3 intersect) const override;

    glm::dvec3 normal() const { return glm::normalize(glm::cross(B - A, C - A)); }

    glm::dvec2 texMapping(glm::dvec3 p) const;

    void setTexCoords(glm::dvec2 Ca, glm::dvec2 Cb, glm::dvec2 Cc);
    void setCoords(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c);

    glm::dvec3 A = {0, 0, 0};
    glm::dvec3 B = {0, 0, 0};
    glm::dvec3 C = {0, 0, 0};

  private:
    void updateCaches();
    /// corner points (ccw)

    /// texture coordinates
    glm::dvec2 tA = {0, 0};
    glm::dvec2 tAB = {0, 0};
    glm::dvec2 tAC = {0, 0};

    glm::dmat2x3 to_tex_map;
};
