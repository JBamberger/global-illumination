#pragma once

#include <Material.h>

struct CheckerboardMaterial final : Material {

    constexpr explicit CheckerboardMaterial(const size_t squares = 10,
                                            const glm::dvec3 color1 = {0.0, 0.0, 0.0},
                                            const glm::dvec3 color2 = {1.0, 1.0, 1.0})
        : square_length(1.0 / squares), color1(color1), color2(color2)
    {
    }

    glm::dvec3 getColor(glm::dvec2 uv) override;

  private:
    const double square_length;
    const glm::dvec3 color1;
    const glm::dvec3 color2;
};