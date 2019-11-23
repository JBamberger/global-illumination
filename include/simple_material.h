#pragma once

#include <material.h>

struct simple_material final : Material {
    constexpr explicit simple_material(const glm::dvec3 color) : color(color) {}

    glm::dvec3 get_color(double u, double v) override;

    glm::dvec3 color;
};