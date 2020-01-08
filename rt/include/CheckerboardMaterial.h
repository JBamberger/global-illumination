#pragma once

#include "NDChecker.h"
#include <Material.h>

class CheckerboardMaterial final : public Material {
    const NdChecker<2> checker_;
    const glm::dvec3 color1_;
    const glm::dvec3 color2_;

  public:
    constexpr explicit CheckerboardMaterial(const size_t squares = 10,
                                            const glm::dvec3 color1 = {0.0, 0.0, 0.0},
                                            const glm::dvec3 color2 = {1.0, 1.0, 1.0})
        : checker_(squares), color1_(color1), color2_(color2)
    {
    }

    glm::dvec3 getColor(const glm::dvec2 uv) override
    {
        return checker_.at({uv.x, uv.y}) ? color2_ : color1_;
    }

    //    glm::dvec3 CheckerboardMaterial::getColor(const glm::dvec2 uv)
    //{
    //    assert(0.0 <= uv.x && uv.x <= 1.0);
    //    assert(0.0 <= uv.y && uv.y <= 1.0);
    //
    //    // TODO: Ensure consistent border values
    //    // consistent for repetition vs. consistency for single texture
    //
    //    // determine if the position is in x or y direction is in an even or odd row
    //    const auto u_even = static_cast<int>(uv.x / square_length) & 1;
    //    const auto v_even = static_cast<int>(uv.y / square_length) & 1;
    //
    //    // the first color is used for all squares that are either both odd or both even
    //    return (u_even && v_even) || !(u_even || v_even) ? color1 : color2;
    //}
};