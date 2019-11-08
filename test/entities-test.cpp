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

struct ImplicitSphereIntersectionTest : testing::Test, testing::WithParamInterface<ray_spec> {

    ImplicitSphereIntersectionTest() { GetParam(); }
};

TEST_P(ImplicitSphereIntersectionTest, test) {
    auto params = GetParam();

    ImplicitSphere sphere;
    sphere.radius = 1;
    glm::dvec3 intersect;
    glm::dvec3 normal;

    Ray ray{{10, 0, 0}, params.direction};

    auto success = sphere.intersect(ray, intersect, normal);
    EXPECT_EQ(params.success, success);
}

INSTANTIATE_TEST_CASE_P(Default,
                        ImplicitSphereIntersectionTest,
                        testing::Values(ray_spec{1, glm::dvec3{-1, 0, 0}},

                                        ray_spec{0, glm::dvec3{-1, 1, 0}},
                                        ray_spec{1, glm::dvec3{-0.04, 0.00401, 0}},
                                        ray_spec{1, glm::dvec3{-0.04, 0.00402, 0}},
                                        ray_spec{0, glm::dvec3{-0.04, 0.00403, 0}},

                                        ray_spec{0, glm::dvec3{-1, -1, 0}},
                                        ray_spec{1, glm::dvec3{-0.04, -0.00401, 0}},
                                        ray_spec{1, glm::dvec3{-0.04, -0.00402, 0}},
                                        ray_spec{0, glm::dvec3{-0.04, -0.00403, 0}},

                                        ray_spec{0, glm::dvec3{-1, 0, 1}},
                                        ray_spec{1, glm::dvec3{-0.04, 0, 0.00401}},
                                        ray_spec{1, glm::dvec3{-0.04, 0, 0.00402}},
                                        ray_spec{0, glm::dvec3{-0.04, 0, 0.00403}},

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