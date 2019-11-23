#pragma once

#include <glm/glm.hpp>

/// Represents the material properties of an entity. For now it only contains color, but it should
/// probably be extended to allow more options.
struct Material {
    constexpr explicit Material(const glm::dvec3 color)
        : ambient(.1), diffuse(1.), specular(1.), glazed(0.), refractive(0.), specular_exponent(20),
          refractive_index(1.), color(color)
    {
    }

    double ambient;
    double diffuse;
    double specular;
    double glazed;
    double refractive;

    double specular_exponent;
    double refractive_index;

    glm::dvec3 color;
};