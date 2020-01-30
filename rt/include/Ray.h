#pragma once

#include <glm/glm.hpp>

struct Ray {
    constexpr static auto offset = 1e-7;

    /// The origination point of the ray.
    glm::dvec3 origin;

    /// Normalized directional vector.
    glm::dvec3 dir;

    /// The number of predecessor rays, i.e. 0 = primary, 1 = secondary, 2 = ternary, ...
    size_t child_level;

    /// The material density where the ray originates.
    double refractive_index;

    /// Creates a new ray but adopts the parent rays properties.
    explicit Ray(const glm::dvec3 origin = glm::dvec3(0, 0, 0),
                 const glm::dvec3 dir = glm::dvec3(1, 0, 0),
                 const size_t child_level = 0,
                 const double refractive_index = 1)
        : origin(origin), dir(glm::normalize(dir)), child_level(child_level),
          refractive_index(refractive_index)
    {
    }

    /// Creates a new ray which is offset a tiny bit in the direction of the ray. This avoids
    /// self-intersections of objects due to numeric instabilities. The properties of the parent ray
    /// are adopted.
    Ray getChildRay(const glm::dvec3 origin, const glm::dvec3 dir) const
    {
        return Ray(origin + dir * offset, dir, child_level + 1, refractive_index);
    }
};
