#include <checkerboard_material.h>

glm::dvec3 checkerboard_material::get_color(const glm::dvec2 uv)
{
    assert(0. <= uv.x && uv.x <= 1.);
    assert(0. <= uv.y && uv.y <= 1.);

    // determine if the position is in x or y direction is in an even or odd row
    const auto u_even = static_cast<int>(uv.x / square_length) & 1;
    const auto v_even = static_cast<int>(uv.y / square_length) & 1;

    // the first color is used for all squares that are either both odd or both even
    if (u_even && v_even || !(u_even || v_even)) {
        return color1;
    }
    return color2;
}
