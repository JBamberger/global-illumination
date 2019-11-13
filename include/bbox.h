#pragma once

#include <array>
#include <cassert>

#include "ray.h"
#include <glm/glm.hpp>

/// Represents an axis-aligned bounding box.
struct BoundingBox {
    BoundingBox(glm::dvec3 min, glm::dvec3 max) : min(min), max(max)
    {
        assert(min.x <= max.x);
        assert(min.y <= max.y);
        assert(min.z <= max.z);
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
        // compute the intersection with each slab given by the box
        const auto mins = (min - ray.origin) / ray.dir;
        const auto maxes = (max - ray.origin) / ray.dir;

        // build valid intervals
        const auto t_mins = glm::min(mins, maxes);
        const auto t_maxes = glm::max(mins, maxes);

        // Intersect first and second interval
        auto t_min = glm::max(t_mins.x, t_mins.y);
        auto t_max = glm::min(t_maxes.x, t_maxes.y);
        if (t_min > t_max)
            return false;

        // Intersect remaining interval with result
        t_min = glm::max(t_min, t_mins.z);
        t_max = glm::min(t_max, t_maxes.z);
        if (t_min > t_max)
            return false;

        // check that the intersection is not behind the ray starting point
        return t_max > 0;
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
