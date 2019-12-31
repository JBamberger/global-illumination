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
    double reflective = 0.0;
    double refractive = 0.0;

    // higher values yield narrower highlights
    double specular_exponent = 20;
    // higher values indicate stronger deflection
    double refractive_index = 1.0;
    // a value gt 0 indicates a rough surface. Secondary ray directions are sampled uniformly on a
    // disk at the tip of the normalized direction vector. The disk has this radius.
    double rough_radius = 0.0;
    // the number of secondary rays for rough surfaces
    size_t reflect_rays = 3;

    /**
     * \brief Returns the color at the given uv coordinates. U and v must be in the range [0,1]
     * \param uv texture coordinates
     * \return the color value at the given position
     */
    virtual glm::dvec3 getColor(glm::dvec2 uv) = 0;
};