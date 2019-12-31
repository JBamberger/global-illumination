#pragma once

#include <Material.h>

struct SimpleMaterial final : Material {
    constexpr explicit SimpleMaterial(const glm::dvec3 color) : color(color) {}

    glm::dvec3 getColor(glm::dvec2 uv) override;

    glm::dvec3 color;
};