#pragma once
#include <array>
#include <cassert>

/**
 * \brief Implements an n-dimensional checkerboard. The checkerboard consumes a location and returns
 * true or false depending if the location is on a black or white square.
 *
 * \tparam N The number of dimensions.
 */
template <size_t N>
class NdChecker {
    const double square_length_;

  public:
    constexpr explicit NdChecker(const size_t squares) : square_length_(1.0 / squares) {}

    /**
     * \brief Determines the board color at the given location.
     *
     * \param pos location at which to evaluate the board
     * \return true if the square is white
     */
    bool at(std::array<double, N> pos) const
    {
        // In a checker board pattern the color changes with any step in any direction. Hence the
        // color can be determined by counting the dimensions with even index and checking if this
        // number is even or odd.

        // determine the number of dimensions where the position falls into an even section
        auto s = 0;
        for (size_t i = 0; i < N; i++) {
            assert(0.0 <= pos[i] && pos[i] <= 1.0);
            s += static_cast<int>(pos[i] / square_length_) & 1;
        }

        return s & 1; // check if the dimensions with even index is even
    }
};
