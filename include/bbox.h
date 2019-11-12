#pragma once

#include <array>
#include <cassert>

#include "ray.h"
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
    glm::dvec3 center() const { return (max + min) / 2.0; }

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

    bool intersect(const Ray& ray) const
    {
        auto t_low = (min - ray.origin) / ray.dir;
        auto t_high = (max - ray.origin) / ray.dir;

        if (ray.dir.x == 0) {
            t_low.x = -std::numeric_limits<double>::infinity();
            t_high.x = std::numeric_limits<double>::infinity();
        }
        if (ray.dir.y == 0) {
            t_low.y = -std::numeric_limits<double>::infinity();
            t_high.y = std::numeric_limits<double>::infinity();
        }
        if (ray.dir.z == 0) {
            t_low.z = -std::numeric_limits<double>::infinity();
            t_high.z = std::numeric_limits<double>::infinity();
        }
        const auto t_min = glm::max(t_low.x, glm::max(t_low.y, t_low.z));
        const auto t_max = glm::min(t_high.x, glm::max(t_high.y, t_high.z));
        return t_min >= t_max;
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
