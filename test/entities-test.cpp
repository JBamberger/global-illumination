#include "glm/gtx/string_cast.hpp"
#include <entities.h>
#include <gtest/gtest.h>
#include <ostream>

struct ray_spec {
    const bool success;
    const glm::dvec3 direction;          // ray direction
    const glm::dvec3 expected_intersect; // intersection point
    const glm::dvec3 expected_normal;    // normal at the intersection point

    ray_spec(const bool s, const glm::dvec3 d, const glm::dvec3 i, const glm::dvec3 n)
        : success(s), direction(glm::normalize(d)), expected_intersect(i),
          expected_normal(glm::normalize(n)) {}
};

struct TriangleIntersectionTest : public testing::TestWithParam<ray_spec> {
    Triangle triangle;
    glm::dvec3 intersect;
    glm::dvec3 normal;

    TriangleIntersectionTest() {
        triangle.A = {0, 0, 0};
        triangle.B = {0, 1, 0};
        triangle.C = {0, 0, 1};
    }
};

TEST_P(TriangleIntersectionTest, testSuccess) {
    auto params = GetParam();
    Ray ray{{10, 0, 0}, params.direction};
    bool success = triangle.intersect(ray, intersect, normal);
    EXPECT_EQ(success, params.success);
}

// clang-format off
INSTANTIATE_TEST_CASE_P(Default, TriangleIntersectionTest, testing::Values(
    ray_spec{1, glm::dvec3{-10,  0.1,  0.1}, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}}, // inside triangle
    ray_spec{0, glm::dvec3{-1,   0,    0  }, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}}, // exact hit test bottom left corner
    ray_spec{0, glm::dvec3{-10, -0.1,  0.1}, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}}, // outside test left
    ray_spec{0, glm::dvec3{-10,  0.1, -0.1}, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}}, // outside test bottom
    ray_spec{0, glm::dvec3{-10,  0.7,  0.7}, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}}, // outside test right
    ray_spec{0, glm::dvec3{-10, -0.1, -0.1}, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}}, // outside test bottom left
    ray_spec{0, glm::dvec3{-10,  1.5, -0.1}, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}}, // outside test bottom right
    ray_spec{0, glm::dvec3{-10, -0.1,  1.5}, glm::dvec3{0,0,0}, glm::dvec3{0,0,0}})  // outside test top right
);
// clang-format on

struct ImplicitSphereIntersectionTest : public testing::TestWithParam<ray_spec> {
    ImplicitSphere sphere;
    glm::dvec3 intersect;
    glm::dvec3 normal;

    ImplicitSphereIntersectionTest() { sphere.radius = 1; }
};

TEST_P(ImplicitSphereIntersectionTest, testSuccess) {
    auto params = GetParam();
    Ray ray{{10, 0, 0}, params.direction};
    bool success = sphere.intersect(ray, intersect, normal);

    EXPECT_EQ(success, params.success);
}

TEST_P(ImplicitSphereIntersectionTest, testIntersect) {
    auto params = GetParam();
    Ray ray{{10, 0, 0}, params.direction};
    sphere.intersect(ray, intersect, normal);

    const double eps = 1e-5;
    if (params.success) {
        EXPECT_NEAR(intersect.x, params.expected_intersect.x, eps);
        EXPECT_NEAR(intersect.y, params.expected_intersect.y, eps);
        EXPECT_NEAR(intersect.z, params.expected_intersect.z, eps);
    }
}

TEST_P(ImplicitSphereIntersectionTest, testNormal) {
    auto params = GetParam();
    Ray ray{{10, 0, 0}, params.direction};
    sphere.intersect(ray, intersect, normal);

    const double eps = 1e-5;
    if (params.success) {
        EXPECT_NEAR(normal.x, params.expected_normal.x, eps);
        EXPECT_NEAR(normal.y, params.expected_normal.y, eps);
        EXPECT_NEAR(normal.z, params.expected_normal.z, eps);
    }
}

// clang-format off
INSTANTIATE_TEST_CASE_P(Default, ImplicitSphereIntersectionTest, testing::Values( 
    ray_spec{1, glm::dvec3{-1,             0,             0           }, glm::dvec3{1,             0,             0           }, glm::dvec3{1,            0,             0            }}, // center
                                                                                                                                                                                      
    ray_spec{0, glm::dvec3{-1,             1,             0           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }}, // right  far outside
    ray_spec{0, glm::dvec3{-1,            -1,             0           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }}, // left   far outside
    ray_spec{0, glm::dvec3{-1,             0,             1           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }}, // top    far outside
    ray_spec{0, glm::dvec3{-1,             0,            -1           }, glm::dvec3{0,             0,             0           }, glm::dvec3{0,            0,             0            }}, // bottom far outside

    ray_spec{1, glm::dvec3{-9.8955634474,  0.9945315513,  0           }, glm::dvec3{0.1044365526,  0.9945315513,  0           }, glm::dvec3{0.0086595509,  0.0861623095,  0           }}, // right  close inside
    ray_spec{1, glm::dvec3{-9.8955634474, -0.9945315513,  0           }, glm::dvec3{0.1044365526, -0.9945315513,  0           }, glm::dvec3{0.0086595509, -0.0861623095,  0           }}, // left   close inside
    ray_spec{1, glm::dvec3{-9.8955634474,  0,             0.9945315513}, glm::dvec3{0.1044365526,  0,             0.9945315513}, glm::dvec3{0.0086595509,  0,             0.0861623095}}, // top    close inside
    ray_spec{1, glm::dvec3{-9.8955634474,  0,            -0.9945315513}, glm::dvec3{0.1044365526,  0,            -0.9945315513}, glm::dvec3{0.0086595509,  0,            -0.0861623095}}, // bottom close inside

    ray_spec{1, glm::dvec3{-9.9,           0.9949874371,  0           }, glm::dvec3{0.1,           0.9949874371,  0           }, glm::dvec3{0.0400670164,  0.3986617801,  0           }}, // right  exact
    ray_spec{1, glm::dvec3{-9.9,          -0.9949874371,  0           }, glm::dvec3{0.1,          -0.9949874371,  0           }, glm::dvec3{0.0400670164, -0.3986617801,  0           }}, // left   exact
    ray_spec{1, glm::dvec3{-9.9,           0,             0.9949874371}, glm::dvec3{0.1,           0,             0.9949874371}, glm::dvec3{0.0400670164,  0,             0.3986617801}}, // top    exact
    ray_spec{1, glm::dvec3{-9.9,           0,            -0.9949874371}, glm::dvec3{0.1,           0,            -0.9949874371}, glm::dvec3{0.0400670164,  0,            -0.3986617801}}, // bottom exact
                                          
    ray_spec{0, glm::dvec3{-0.04,          0.00403,       0           }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }}, // right  close outside
    ray_spec{0, glm::dvec3{-0.04,         -0.00403,       0           }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }}, // left   close outside
    ray_spec{0, glm::dvec3{-0.04,          0,             0.00403     }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }}, // top    close outside
    ray_spec{0, glm::dvec3{-0.04,          0,            -0.00403     }, glm::dvec3{0,             0,            0            }, glm::dvec3{0,             0,             0           }}) // bottom close outside
);
// clang-format on

TEST(ImplicitSphere, center) {
    ImplicitSphere s1;
    s1.radius = 1;

    glm::dvec3 a, b;
    Ray r1{{10, 0, 0}, {-1, 0, 0}};

    ASSERT_TRUE(s1.intersect(r1, a, b));
}