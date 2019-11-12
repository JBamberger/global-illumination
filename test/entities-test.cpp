#include <entities.h>
#include <gtest/gtest.h>
#include <ostream>
#include <string>

constexpr static auto eps = 1e-5;

struct ray_spec {
    const bool success;
    const glm::dvec3 direction;          // ray direction
    const glm::dvec3 expected_intersect; // intersection point
    const glm::dvec3 expected_normal;    // normal at the intersection point
    const std::string description;       // test case name

    ray_spec(const bool s,
             const glm::dvec3 d,
             const glm::dvec3 i,
             const glm::dvec3 n,
             const std::string description)
        : success(s), direction(glm::normalize(d)), expected_intersect(i),
          expected_normal(glm::normalize(n)), description(description)
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

struct TriangleIntersectionTest : public testing::TestWithParam<ray_spec> {
    triangle triangle;
    glm::dvec3 intersect;
    glm::dvec3 normal;

    TriangleIntersectionTest()
    {
        triangle.A = {0, 0, 0};
        triangle.B = {0, 1, 0};
        triangle.C = {0, 0, 1};
    }
};

TEST_P(TriangleIntersectionTest, testSuccess)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    const auto success = triangle.intersect(ray, intersect, normal);
    EXPECT_EQ(success, params.success);
}

TEST_P(TriangleIntersectionTest, testIntersect)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    triangle.intersect(ray, intersect, normal);

    if (params.success) {
        EXPECT_NEAR(intersect.x, params.expected_intersect.x, eps);
        EXPECT_NEAR(intersect.y, params.expected_intersect.y, eps);
        EXPECT_NEAR(intersect.z, params.expected_intersect.z, eps);
    }
}

TEST_P(TriangleIntersectionTest, testNormal)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    triangle.intersect(ray, intersect, normal);

    if (params.success) {
        EXPECT_NEAR(normal.x, params.expected_normal.x, eps);
        EXPECT_NEAR(normal.y, params.expected_normal.y, eps);
        EXPECT_NEAR(normal.z, params.expected_normal.z, eps);
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

struct ImplicitSphereIntersectionTest : public testing::TestWithParam<ray_spec> {
    implicit_sphere sphere;
    glm::dvec3 intersect;
    glm::dvec3 normal;

    ImplicitSphereIntersectionTest() { sphere.radius = 1; }
};

TEST_P(ImplicitSphereIntersectionTest, testSuccess)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    const auto success = sphere.intersect(ray, intersect, normal);

    EXPECT_EQ(success, params.success);
}

TEST_P(ImplicitSphereIntersectionTest, testIntersect)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    sphere.intersect(ray, intersect, normal);

    if (params.success) {
        EXPECT_NEAR(intersect.x, params.expected_intersect.x, eps);
        EXPECT_NEAR(intersect.y, params.expected_intersect.y, eps);
        EXPECT_NEAR(intersect.z, params.expected_intersect.z, eps);
    }
}

TEST_P(ImplicitSphereIntersectionTest, testNormal)
{
    const auto params = GetParam();
    const Ray ray{{10, 0, 0}, params.direction};
    sphere.intersect(ray, intersect, normal);

    if (params.success) {
        EXPECT_NEAR(normal.x, params.expected_normal.x, eps);
        EXPECT_NEAR(normal.y, params.expected_normal.y, eps);
        EXPECT_NEAR(normal.z, params.expected_normal.z, eps);
    }
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(InsideOutside, ImplicitSphereIntersectionTest, testing::Values(
    ray_spec{true,  glm::dvec3{-1,             0,             0           }, glm::dvec3{1,             0,             0           }, glm::dvec3{1,            0,             0            }, "center"			   },

    ray_spec{false, glm::dvec3{-1,             1,             0           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }, "right  far outside"  },
    ray_spec{false, glm::dvec3{-1,            -1,             0           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }, "left   far outside"  },
    ray_spec{false, glm::dvec3{-1,             0,             1           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }, "top    far outside"  },
    ray_spec{false, glm::dvec3{-1,             0,            -1           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }, "bottom far outside"  }
));
INSTANTIATE_TEST_SUITE_P(CloseToBorder, ImplicitSphereIntersectionTest, testing::Values(
    ray_spec{true,  glm::dvec3{-9.8955634474,  0.9945315513,  0           }, glm::dvec3{0.1044365526,  0.9945315513,  0           }, glm::dvec3{0.1021811887,  0.9730540950,  0           }, "right  close inside" },
    ray_spec{true,  glm::dvec3{-9.8955634474, -0.9945315513,  0           }, glm::dvec3{0.1044365526, -0.9945315513,  0           }, glm::dvec3{0.1021811887, -0.9730540950,  0           }, "left   close inside" },
    ray_spec{true,  glm::dvec3{-9.8955634474,  0,             0.9945315513}, glm::dvec3{0.1044365526,  0,             0.9945315513}, glm::dvec3{0.1021811887,  0,             0.9730540950}, "top    close inside" },
    ray_spec{true,  glm::dvec3{-9.8955634474,  0,            -0.9945315513}, glm::dvec3{0.1044365526,  0,            -0.9945315513}, glm::dvec3{0.1021811887,  0,            -0.9730540950}, "bottom close inside" },

    ray_spec{true,  glm::dvec3{-9.9,           0.9949874371,  0           }, glm::dvec3{0.1,           0.9949874371,  0           }, glm::dvec3{0.0400670164,  0.3986617801,  0           }, "right  exact"		   },
    ray_spec{true,  glm::dvec3{-9.9,          -0.9949874371,  0           }, glm::dvec3{0.1,          -0.9949874371,  0           }, glm::dvec3{0.0400670164, -0.3986617801,  0           }, "left   exact"		   },
    ray_spec{true,  glm::dvec3{-9.9,           0,             0.9949874371}, glm::dvec3{0.1,           0,             0.9949874371}, glm::dvec3{0.0400670164,  0,             0.3986617801}, "top    exact"		   },
    ray_spec{true,  glm::dvec3{-9.9,           0,            -0.9949874371}, glm::dvec3{0.1,           0,            -0.9949874371}, glm::dvec3{0.0400670164,  0,            -0.3986617801}, "bottom exact"		   },

    ray_spec{false, glm::dvec3{-0.04,          0.00403,       0           }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }, "right  close outside"},
    ray_spec{false, glm::dvec3{-0.04,         -0.00403,       0           }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }, "left   close outside"},
    ray_spec{false, glm::dvec3{-0.04,          0,             0.00403     }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }, "top    close outside"},
    ray_spec{false, glm::dvec3{-0.04,          0,            -0.00403     }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }, "bottom close outside"}
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