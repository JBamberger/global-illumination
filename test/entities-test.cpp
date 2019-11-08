#include "glm/gtx/string_cast.hpp"
#include <entities.h>
#include <gtest/gtest.h>
#include <ostream>

struct ray_spec {
    bool success;
    glm::dvec3 direction;

    friend std::ostream& operator<<(std::ostream& os, const ray_spec& obj) {
        return os << "success: " << obj.success << " direction: " << glm::to_string(obj.direction);
    }
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
    EXPECT_EQ(params.success, success);
}

INSTANTIATE_TEST_CASE_P(Default,
                        TriangleIntersectionTest,
                        testing::Values( // inside triangle
                            ray_spec{1, glm::dvec3{-10, 0.1, 0.1}},
                            // exact border hits
                            ray_spec{0, glm::dvec3{-1, 0, 0}}, // bottom left corner
                            // outside tests
                            ray_spec{0, glm::dvec3{-10, -0.1, 0.1}},  // left
                            ray_spec{0, glm::dvec3{-10, 0.1, -0.1}},  // bottom
                            ray_spec{0, glm::dvec3{-10, 0.7, 0.7}},   // right
                            ray_spec{0, glm::dvec3{-10, -0.1, -0.1}}, // bottom left
                            ray_spec{0, glm::dvec3{-10, 1.5, -0.1}},  // bottom right
                            ray_spec{0, glm::dvec3{-10, -0.1, 1.5}}   // top right
                            ));

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

    EXPECT_EQ(params.success, success);
}

INSTANTIATE_TEST_CASE_P(Default,
                        ImplicitSphereIntersectionTest,
                        testing::Values( // center
                            ray_spec{1, glm::dvec3{-1, 0, 0}},
                            // right
                            ray_spec{0, glm::dvec3{-1, 1, 0}},
                            ray_spec{1, glm::dvec3{-0.04, 0.00401, 0}},
                            ray_spec{1, glm::dvec3{-0.04, 0.00402, 0}},
                            ray_spec{0, glm::dvec3{-0.04, 0.00403, 0}},
                            // left
                            ray_spec{0, glm::dvec3{-1, -1, 0}},
                            ray_spec{1, glm::dvec3{-0.04, -0.00401, 0}},
                            ray_spec{1, glm::dvec3{-0.04, -0.00402, 0}},
                            ray_spec{0, glm::dvec3{-0.04, -0.00403, 0}},
                            // top
                            ray_spec{0, glm::dvec3{-1, 0, 1}},
                            ray_spec{1, glm::dvec3{-0.04, 0, 0.00401}},
                            ray_spec{1, glm::dvec3{-0.04, 0, 0.00402}},
                            ray_spec{0, glm::dvec3{-0.04, 0, 0.00403}},
                            // bottom
                            ray_spec{0, glm::dvec3{-1, 0, -1}},
                            ray_spec{1, glm::dvec3{-0.04, 0, -0.00401}},
                            ray_spec{1, glm::dvec3{-0.04, 0, -0.00402}},
                            ray_spec{0, glm::dvec3{-0.04, 0, -0.00403}}));

TEST(ImplicitSphere, center) {
    ImplicitSphere s1;
    s1.radius = 1;

    glm::dvec3 a, b;
    Ray r1{{10, 0, 0}, {-1, 0, 0}};

    ASSERT_TRUE(s1.intersect(r1, a, b));
}