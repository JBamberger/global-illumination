#pragma once

#include <array>
#include <cassert>

#include <glm/glm.hpp>

/// Represents an axis-aligned bounding box.
struct BoundingBox {
    BoundingBox(glm::dvec3 min, glm::dvec3 max) : min(min), max(max)
    {
        assert(min.x < max.x);
        assert(min.y < max.y);
        assert(min.z < max.z);
    }

    double dx() const { return max.x - min.x; }
    double dy() const { return max.y - min.y; }
    double dz() const { return max.z - min.z; }

    const glm::dvec3 min;
    const glm::dvec3 max;

    /// Check if another bounding box intersects the current bounding box.
    bool intersect(const BoundingBox& other) const
    {
        // Check if the intervals in each dimension overlap (slab intersection)
        return (min.x <= other.max.x && other.min.x <= max.x) && // test x dimension
               (min.y <= other.max.y && other.min.y <= max.y) && // test y dimension
               (min.z <= other.max.z && other.min.z <= max.z);   // test z dimension
    }

    /// Check if a point lies within the bounding box.
    bool contains(glm::dvec3 point) const
    {
        // basically the same as the bbox test but min and max are the same values
        return (min.x <= point.x && point.x <= max.x) && // test x dimension
               (min.y <= point.y && point.y <= max.y) && // test y dimension
               (min.z <= point.z && point.z <= max.z);   // test z dimension
    }

    static BoundingBox unite(const BoundingBox& b1, const BoundingBox& b2)
    {
        return BoundingBox{glm::min(b1.min, b2.min), glm::max(b1.max, b2.max)};
    }
};
