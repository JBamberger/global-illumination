
#include "Material.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <gtest/gtest.h>
#include <ostream>
#include <string>
#include <utility>

constexpr glm::dvec3 white{1, 1, 1};
constexpr glm::dvec3 black{0, 0, 0};

struct coord_def {
    const glm::dvec2 uv;
    const glm::dvec3 color;
    const std::string description;

    coord_def(const glm::dvec2 uv, const glm::dvec3 color, std::string description)
        : uv(uv), color(color), description(std::move(description))
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const coord_def& obj)
    {
        return os << obj.description;
    }
};

struct CheckerboardAccessTest : public testing::TestWithParam<coord_def> {
    CheckerboardMaterial material;

    CheckerboardAccessTest() : material() {}
};

TEST_P(CheckerboardAccessTest, testCheckerboarAcces)
{
    const auto p = GetParam();
    const auto color = material.value(p.uv);

    ASSERT_EQ(color, p.color);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Default, CheckerboardAccessTest, testing::Values(

	// TODO: enable border checks
	//coord_def{{0.0, 0.0}, black, "top right corner"   },
	//coord_def{{1.0, 0.0}, white, "top left corner"    },
	//coord_def{{0.0, 1.0}, white, "bottom right corner"},
	//coord_def{{1.0, 1.0}, black, "bottom left corner" },

	coord_def{{0.1, 0.0}, white, "horizontal border 1st, 2nd" },
	coord_def{{0.0, 0.1}, white, "vertical border 1st, 2nd"   },

	coord_def{{0.05, 0.05}, black, "center 0,0"   },

	coord_def{{0.15, 0.05}, white, "center 1,0"   },
	coord_def{{0.25, 0.05}, black, "center 2,0"   },
	coord_def{{0.35, 0.05}, white, "center 3,0"   },
	coord_def{{0.45, 0.05}, black, "center 4,0"   },
	coord_def{{0.55, 0.05}, white, "center 5,0"   },
	coord_def{{0.65, 0.05}, black, "center 6,0"   },
	coord_def{{0.75, 0.05}, white, "center 7,0"   },
	coord_def{{0.85, 0.05}, black, "center 8,0"   },
	coord_def{{0.95, 0.05}, white, "center 9,0"   },

	coord_def{{0.05, 0.15}, white, "center 0,1"   },
	coord_def{{0.05, 0.25}, black, "center 0,2"   },
	coord_def{{0.05, 0.35}, white, "center 0,3"   },
	coord_def{{0.05, 0.45}, black, "center 0,4"   },
	coord_def{{0.05, 0.55}, white, "center 0,5"   },
	coord_def{{0.05, 0.65}, black, "center 0,6"   },
	coord_def{{0.05, 0.75}, white, "center 0,7"   },
	coord_def{{0.05, 0.85}, black, "center 0,8"   },
	coord_def{{0.05, 0.95}, white, "center 0,9"   },

	coord_def{{0.15, 0.15}, black, "center 1,1"   },
	coord_def{{0.25, 0.25}, black, "center 2,2"   },
	coord_def{{0.35, 0.35}, black, "center 3,3"   },
	coord_def{{0.45, 0.45}, black, "center 4,4"   },
	coord_def{{0.55, 0.55}, black, "center 5,5"   },
	coord_def{{0.65, 0.65}, black, "center 6,6"   },
	coord_def{{0.75, 0.75}, black, "center 7,7"   },
	coord_def{{0.85, 0.85}, black, "center 8,8"   },
	coord_def{{0.95, 0.95}, black, "center 9,9"   }/*,

	coord_def{{0.1, 0.0}, white, "border 1,0"   },
	coord_def{{0.2, 0.0}, black, "border 2,0"   },
	coord_def{{0.3, 0.0}, white, "border 3,0"   },
	coord_def{{0.4, 0.0}, black, "border 4,0"   },
	coord_def{{0.5, 0.0}, white, "border 5,0"   },
	coord_def{{0.6, 0.0}, black, "border 6,0"   },
	coord_def{{0.7, 0.0}, white, "border 7,0"   },
	coord_def{{0.8, 0.0}, black, "border 8,0"   },
	coord_def{{0.9, 0.0}, white, "border 9,0"   },

	coord_def{{0.0, 0.1}, white, "border 0,1"   },
	coord_def{{0.0, 0.2}, black, "border 0,2"   },
	coord_def{{0.0, 0.3}, white, "border 0,3"   },
	coord_def{{0.0, 0.4}, black, "border 0,4"   },
	coord_def{{0.0, 0.5}, white, "border 0,5"   },
	coord_def{{0.0, 0.6}, black, "border 0,6"   },
	coord_def{{0.0, 0.7}, white, "border 0,7"   },
	coord_def{{0.0, 0.8}, black, "border 0,8"   },
	coord_def{{0.0, 0.9}, white, "border 0,9"   },

	coord_def{{0.1, 0.1}, black, "border 1,1"   },
	coord_def{{0.2, 0.2}, black, "border 2,2"   },
	coord_def{{0.3, 0.3}, black, "border 3,3"   },
	coord_def{{0.4, 0.4}, black, "border 4,4"   },
	coord_def{{0.5, 0.5}, black, "border 5,5"   },
	coord_def{{0.6, 0.6}, black, "border 6,6"   },
	coord_def{{0.7, 0.7}, black, "border 7,7"   },
	coord_def{{0.8, 0.8}, black, "border 8,8"   },
	coord_def{{0.9, 0.9}, black, "border 9,9"   }*/
));
// clang-format on
