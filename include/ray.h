#pragma once

#include <glm/glm.hpp>

constexpr auto offset = 1e-7;

struct Ray {
    Ray(const glm::dvec3 origin, const glm::dvec3 dir) : origin(origin), dir(glm::normalize(dir)) {}

    glm::dvec3 origin;
    glm::dvec3 dir; // normalized directional vector
    double refractive_index = 1;

    /// Creates a new ray which is offset a tiny bit in the direction of the ray. This avoids
    /// self-intersections of objects due to numeric instabilities.
    static Ray offset_ray(const glm::dvec3 origin, const glm::dvec3 dir)
    {
        return Ray{origin + dir * offset, dir};
    }
};
