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

#include "entities.h"

#include <glm/vec3.hpp>
#include <gtest/gtest.h>
#include <ostream>
#include <string>
#include <utility>

constexpr static auto eps = 1e-5;

/**
 * \brief Struct describing a single test case. It contains the ray direction of a ray originating
 * in (10,0,0) and the expected intersection point and normal.
 */
struct ray_spec {
    const bool success;                  // true if there is an intersection
    const glm::dvec3 direction;          // normalized ray direction
    const glm::dvec3 expected_intersect; // intersection point
    const glm::dvec3 expected_normal;    // normalized normal at the intersection point
    const std::string description;       // test case name

    ray_spec(const bool s,
             const glm::dvec3 d,
             const glm::dvec3 i,
             const glm::dvec3 n,
             std::string description)
        : success(s), direction(glm::normalize(d)), expected_intersect(i),
          expected_normal(glm::normalize(n)), description(std::move(description))
    {
    }

    ray_spec(const bool s, const glm::dvec3 d, const glm::dvec3 i, const glm::dvec3 n)
        : ray_spec(s, d, i, n, "Rayspec test")
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const ray_spec& obj)
    {
        return os << obj.description;
    }
};

//#####################################################################################################################
// Triangle intersection tests
//#####################################################################################################################

struct TriangleIntersectionTest : testing::TestWithParam<ray_spec> {
    Triangle triangle;
    Hit hit;

    TriangleIntersectionTest()
        : triangle(glm::dvec3{0, 0, 0}, glm::dvec3{0, 1, 0}, glm::dvec3{0, 0, 1})
    {
    }
};

TEST_P(TriangleIntersectionTest, testSuccess)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    const auto success = triangle.intersect(ray, hit);
    EXPECT_EQ(success, params.success);
}

TEST_P(TriangleIntersectionTest, testIntersect)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    static_cast<void>(triangle.intersect(ray, hit));

    if (params.success) {
        EXPECT_NEAR(hit.pos.x, params.expected_intersect.x, eps);
        EXPECT_NEAR(hit.pos.y, params.expected_intersect.y, eps);
        EXPECT_NEAR(hit.pos.z, params.expected_intersect.z, eps);
    }
}

TEST_P(TriangleIntersectionTest, testNormal)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    static_cast<void>(triangle.intersect(ray, hit));

    if (params.success) {
        EXPECT_NEAR(hit.normal.x, params.expected_normal.x, eps);
        EXPECT_NEAR(hit.normal.y, params.expected_normal.y, eps);
        EXPECT_NEAR(hit.normal.z, params.expected_normal.z, eps);
    }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InsideOutside, TriangleIntersectionTest, testing::Values(
    ray_spec{true,  {-10,  0.1,  0.1}, {0, 0.1, 0.1}, {1, 0, 0}, "inside triangle"                  },
    ray_spec{true,  {-1,   0,    0  }, {0, 0,   0  }, {1, 0, 0}, "exact hit test bottom left corner"},
    ray_spec{false, {-10, -0.1,  0.1}, {0, 0,   0  }, {0, 0, 0}, "outside test left"                },
    ray_spec{false, {-10,  0.1, -0.1}, {0, 0,   0  }, {0, 0, 0}, "outside test bottom"              },
    ray_spec{false, {-10,  0.7,  0.7}, {0, 0,   0  }, {0, 0, 0}, "outside test right"               },
    ray_spec{false, {-10, -0.1, -0.1}, {0, 0,   0  }, {0, 0, 0}, "outside test bottom left"         },
    ray_spec{false, {-10,  1.5, -0.1}, {0, 0,   0  }, {0, 0, 0}, "outside test bottom right"        },
    ray_spec{false, {-10, -0.1,  1.5}, {0, 0,   0  }, {0, 0, 0}, "outside test top right"           })
);
// clang-format on

//#####################################################################################################################
// Implicit sphere intersection tests
//#####################################################################################################################

struct ImplicitSphereIntersectionTest : testing::TestWithParam<ray_spec> {
    Sphere sphere;
    Hit hit;

    ImplicitSphereIntersectionTest() { sphere.radius = 1; }
};

TEST_P(ImplicitSphereIntersectionTest, testSuccess)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    const auto success = sphere.intersect(ray, hit);

    EXPECT_EQ(success, params.success);
}

TEST_P(ImplicitSphereIntersectionTest, testIntersect)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    static_cast<void>(sphere.intersect(ray, hit));

    if (params.success) {
        EXPECT_NEAR(hit.pos.x, params.expected_intersect.x, eps);
        EXPECT_NEAR(hit.pos.y, params.expected_intersect.y, eps);
        EXPECT_NEAR(hit.pos.z, params.expected_intersect.z, eps);
    }
}

TEST_P(ImplicitSphereIntersectionTest, testNormal)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    static_cast<void>(sphere.intersect(ray, hit));

    if (params.success) {
        EXPECT_NEAR(hit.normal.x, params.expected_normal.x, eps);
        EXPECT_NEAR(hit.normal.y, params.expected_normal.y, eps);
        EXPECT_NEAR(hit.normal.z, params.expected_normal.z, eps);
    }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InsideOutside, ImplicitSphereIntersectionTest, testing::Values(
    ray_spec{true,  {-1,  0,  0}, {1, 0, 0}, {1, 0, 0}, "center"            },						  			 
    ray_spec{false, {-1,  1,  0}, {0, 0, 0}, {0, 0, 0}, "right  far outside"},
    ray_spec{false, {-1, -1,  0}, {0, 0, 0}, {0, 0, 0}, "left   far outside"},
    ray_spec{false, {-1,  0,  1}, {0, 0, 0}, {0, 0, 0}, "top    far outside"},
    ray_spec{false, {-1,  0, -1}, {0, 0, 0}, {0, 0, 0}, "bottom far outside"}
));

INSTANTIATE_TEST_SUITE_P(CloseToBorder, ImplicitSphereIntersectionTest, testing::Values(
    ray_spec{true,  {-9.8955634474,  0.9945315513,  0           }, {0.1044365526,  0.9945315513,  0           }, {0.1021811887,  0.9730540950,  0           }, "right  close inside" },
    ray_spec{true,  {-9.8955634474, -0.9945315513,  0           }, {0.1044365526, -0.9945315513,  0           }, {0.1021811887, -0.9730540950,  0           }, "left   close inside" },
    ray_spec{true,  {-9.8955634474,  0,             0.9945315513}, {0.1044365526,  0,             0.9945315513}, {0.1021811887,  0,             0.9730540950}, "top    close inside" },
    ray_spec{true,  {-9.8955634474,  0,            -0.9945315513}, {0.1044365526,  0,            -0.9945315513}, {0.1021811887,  0,            -0.9730540950}, "bottom close inside" },

    ray_spec{true,  {-9.9,           0.9949874371,  0           }, {0.1,           0.9949874371,  0           }, {0.0400670164,  0.3986617801,  0           }, "right  exact"        },
    ray_spec{true,  {-9.9,          -0.9949874371,  0           }, {0.1,          -0.9949874371,  0           }, {0.0400670164, -0.3986617801,  0           }, "left   exact"        },
    ray_spec{true,  {-9.9,           0,             0.9949874371}, {0.1,           0,             0.9949874371}, {0.0400670164,  0,             0.3986617801}, "top    exact"        },
    ray_spec{true,  {-9.9,           0,            -0.9949874371}, {0.1,           0,            -0.9949874371}, {0.0400670164,  0,            -0.3986617801}, "bottom exact"        },

    ray_spec{false, {-0.04,          0.00403,       0           }, {0,             0,            0            }, {0,             0,             0           }, "right  close outside"},
    ray_spec{false, {-0.04,         -0.00403,       0           }, {0,             0,            0            }, {0,             0,             0           }, "left   close outside"},
    ray_spec{false, {-0.04,          0,             0.00403     }, {0,             0,            0            }, {0,             0,             0           }, "top    close outside"},
    ray_spec{false, {-0.04,          0,            -0.00403     }, {0,             0,            0            }, {0,             0,             0           }, "bottom close outside"}
));
// clang-format on

// TEST(ImplicitSphere, center)
//{
//    implicit_sphere s1;
//    s1.radius = 1;
//
//    glm::dvec3 a, b;
//    Ray r1{{10, 0, 0}, {-1, 0, 0}};
//
//    ASSERT_TRUE(s1.intersect(r1, a, b));
//}
