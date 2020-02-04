#pragma once

#include "Ray.h"
#include <glm/glm.hpp>

/// Represents an axis-aligned bounding box.
struct BoundingBox {

    glm::dvec3 min;
    glm::dvec3 max;

    BoundingBox(glm::dvec3 min, glm::dvec3 max);

    /// Check if another bounding box intersects the current bounding box.
    bool intersect(const BoundingBox& other) const;

    /// Check if a ray intersects with the box in any location.
    bool intersect(const Ray& ray) const;

    /// Check if a point lies within the bounding box.
    bool contains(glm::dvec3 point) const;

    /// Computes the minimal bbox which contains b1 and b2
    static BoundingBox unite(const BoundingBox& b1, const BoundingBox& b2);
};
