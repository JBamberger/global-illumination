#pragma once

#include <glm/glm.hpp>

/// Represents the material properties of an entity. For now it only contains color, but it should
/// probably be extended to allow more options.
struct Material {
    constexpr explicit Material(const glm::dvec3 color)
        : ambient(.1), diffuse(1.), specular(1.), specular_exponent(20), glazed(0.), color(color)
    {
    }

    double ambient;
    double diffuse;
    double specular;
    double specular_exponent;
    double glazed;
    glm::dvec3 color;
};