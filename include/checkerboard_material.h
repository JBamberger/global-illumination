#pragma once

#include <material.h>

struct checkerboard_material final : Material {

    constexpr explicit checkerboard_material(const size_t squares = 10,
                                             const glm::dvec3 color1 = {0.0, 0.0, 0.0},
                                             const glm::dvec3 color2 = {1.0, 1.0, 1.0})
        : square_length(1.0 / squares), color1(color1), color2(color2)
    {
    }

    glm::dvec3 get_color(glm::dvec2 uv) override;

  private:
    const double square_length;
    const glm::dvec3 color1;
    const glm::dvec3 color2;
};