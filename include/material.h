#pragma once

#include <glm/glm.hpp>

/// Represents the material properties of an entity. For now it only contains color, but it should
/// probably be extended to allow more options.
struct Material {
    constexpr explicit Material() = default;
    virtual ~Material() = default;

    double ambient = 0.1;
    double diffuse = 1.0;
    double specular = 1.0;
    double glazed = 0.0;
    double refractive = 0.0;

    double specular_exponent = 20;
    double refractive_index = 1.0;

    /**
     * \brief Returns the color at the given uv coordinates. U and v must be in the range [0,1]
     * \param u u-coordinate of the texture
     * \param v v-coordinate of the texture
     * \return the color value at the given position
     */
    virtual glm::dvec3 get_color(glm::dvec2 uv) = 0;
};