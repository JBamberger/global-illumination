#include <gtest/gtest.h>

#include "bbox.h"
#include <ostream>
#include <string>
#include <utility>

struct case_definition {
    const bool success;
    const Ray ray;
    const std::string description;

    case_definition(const bool success,
                    const glm::dvec3 origin,
                    const glm::dvec3 direction,
                    const std::string description)
        : success(success), ray{origin, direction}, description(description)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const case_definition& obj)
    {
        return os << obj.description;
    }
};

struct RayIntersectionTest : public testing::TestWithParam<case_definition> {
    BoundingBox bb;

    RayIntersectionTest() : bb(glm::dvec3{-1, -1, -1}, glm::dvec3{1, 1, 1}) {}
};

TEST_P(RayIntersectionTest, checkRay)
{
    const auto p = GetParam();
    const auto success = bb.intersect(p.ray);

    ASSERT_EQ(success, p.success);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Default, RayIntersectionTest, testing::Values(
	case_definition{true,  { 2,  0,  0}, {-1,  0,  0}, "front into the box"       },
	case_definition{true,  {-2,  0,  0}, { 1,  0,  0}, "back into the box"        },
	case_definition{true,  { 0,  2,  0}, { 0, -1,  0}, "right into the box"       },
	case_definition{true,  { 0, -2,  0}, { 0,  1,  0}, "left into the box"        },
	case_definition{true,  { 0,  0,  2}, { 0,  0, -1}, "top into the box"         },
	case_definition{true,  { 0,  0, -2}, { 0,  0,  1}, "bottom into the box"      },

	case_definition{false, { 2,  0,  0}, { 1,  0,  0}, "front away from the box"  },
	case_definition{false, {-2,  0,  0}, {-1,  0,  0}, "back away from the box"   },
	case_definition{false, { 0,  2,  0}, { 0,  1,  0}, "right away from the box"  },
	case_definition{false, { 0, -2,  0}, { 0, -1,  0}, "left away from the box"   },
	case_definition{false, { 0,  0,  2}, { 0,  0,  1}, "top away from the box"    },
	case_definition{false, { 0,  0, -2}, { 0,  0, -1}, "bottom away from the box" },

	case_definition{false, { 2,  2,  2}, {-1,  0,  0}, "parallel along x box"     },
	case_definition{false, { 2,  2,  2}, { 0, -1,  0}, "parallel along y box"     },
	case_definition{false, { 2,  2,  2}, { 0,  0, -1}, "parallel along z box"     },

	case_definition{true,  { 0,  2,  1}, { 0, -4, -2}, "through left and right"   },
	case_definition{true,  { 0,  2,  1}, { 0, -3, -3}, "through bottom and right" },
	case_definition{true,  { 2,  2,  2}, {-1, -1, -1}, "diagonal through the box" },
	case_definition{true,  { 0,  0,  0}, { 1,  1,  1}, "from within the box"      },

	case_definition{true,  { 2,  0,  0}, {-1, -0, -0}, "front of box neg. zero"   }
));
// clang-format on
