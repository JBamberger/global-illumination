/**
 *    Copyright 2020 Jannik Bamberger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Entity.h"
#include <gtest/gtest.h>
#include <ostream>
#include <string>
#include <utility>

constexpr glm::dvec3 black{0, 0, 0};
constexpr glm::dvec3 white{1, 1, 1};

struct mapping_def {
    const glm::dvec3 intersect;
    const glm::dvec2 uv;
    const std::string description;

    mapping_def(const glm::dvec3 intersect, const glm::dvec2 uv, std::string description)
        : intersect(intersect), uv(uv), description(std::move(description))
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
    Triangle t;

    TextureMappingTest() : t({0, -1, 0}, {0, 1, 0}, {0, 0, 1})
    {
        t.setTexCoords(glm::dvec2{0, 1}, glm::dvec2{1, 1}, glm::dvec2{0.5, 0});
    }
};

TEST_P(TextureMappingTest, testUVMapping)
{
    const auto p = GetParam();
    const auto uv = t.texMapping(p.intersect);

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
