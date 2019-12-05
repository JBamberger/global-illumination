#include <gtest/gtest.h>

#include "checkerboard_material.h"
#include "triangle.h"
#include <ostream>
#include <string>
#include <utility>

static const glm::dvec3 black{0, 0, 0};
static const glm::dvec3 white{1, 1, 1};

struct mapping_def {
    const glm::dvec3 intersect;
    const glm::dvec2 uv;
    const std::string description;

    mapping_def(const glm::dvec3 intersect, const glm::dvec2 uv, const std::string description)
        : intersect(intersect), uv(uv), description(description)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const mapping_def& obj)
    {
        return os << obj.description;
    }
};

std::ostream& operator<<(std::ostream& os, const glm::dvec2& obj)
{
    return os << "(" << obj.x << "," << obj.y << ")";
}

struct TextureMappingTest : public testing::TestWithParam<mapping_def> {
    triangle t;

    TextureMappingTest() : t({0, -1, 0}, {0, 1, 0}, {0, 0, 1})
    {
        t.t1 = {0, 1};
        t.t2 = {1, 1};
        t.t3 = {0.5, 0};
    }
};

TEST_P(TextureMappingTest, testUVMapping)
{
    const auto p = GetParam();
    const auto uv = t.tex_mapping(p.intersect);

    ASSERT_EQ(uv, p.uv);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Default, TextureMappingTest, testing::Values(

	mapping_def{{0, -1, 0},   {0.0, 1.0},  "lower left corner"  },
	mapping_def{{0,  1, 0},   {1.0, 1.0},  "lower right corner" },
	mapping_def{{0,  0, 1},   {0.5, 0.0},  "top corner"         },

	mapping_def{{0,  0, 0},   {0.5, 1.0},  "bottom center"      },
	mapping_def{{0,  0, 0.5}, {0.5, 0.5},  "center"             }
));
// clang-format on
