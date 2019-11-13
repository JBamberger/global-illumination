#include <gtest/gtest.h>

#include "bbox.h"
#include <ostream>
#include <string>
#include <utility>

struct ray_definition {
    const bool success;
    const Ray ray;
    const std::string description;

    ray_definition(const bool success,
                   const glm::dvec3 origin,
                   const glm::dvec3 direction,
                   const std::string description)
        : success(success), ray{origin, direction}, description(description)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const ray_definition& obj)
    {
        return os << obj.description;
    }
};

struct RayBBoxIntersectionTest : public testing::TestWithParam<ray_definition> {
    BoundingBox bb;

    RayBBoxIntersectionTest() : bb(glm::dvec3{-1, -1, -1}, glm::dvec3{1, 1, 1}) {}
};

TEST_P(RayBBoxIntersectionTest, testRayIntersection)
{
    const auto p = GetParam();
    const auto success = bb.intersect(p.ray);

    ASSERT_EQ(success, p.success);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Default, RayBBoxIntersectionTest, testing::Values(
	ray_definition{true,  { 2,  0,  0}, {-1,  0,  0}, "front into the box"       },
	ray_definition{true,  {-2,  0,  0}, { 1,  0,  0}, "back into the box"        },
	ray_definition{true,  { 0,  2,  0}, { 0, -1,  0}, "right into the box"       },
	ray_definition{true,  { 0, -2,  0}, { 0,  1,  0}, "left into the box"        },
	ray_definition{true,  { 0,  0,  2}, { 0,  0, -1}, "top into the box"         },
	ray_definition{true,  { 0,  0, -2}, { 0,  0,  1}, "bottom into the box"      },

	ray_definition{false, { 2,  0,  0}, { 1,  0,  0}, "front away from the box"  },
	ray_definition{false, {-2,  0,  0}, {-1,  0,  0}, "back away from the box"   },
	ray_definition{false, { 0,  2,  0}, { 0,  1,  0}, "right away from the box"  },
	ray_definition{false, { 0, -2,  0}, { 0, -1,  0}, "left away from the box"   },
	ray_definition{false, { 0,  0,  2}, { 0,  0,  1}, "top away from the box"    },
	ray_definition{false, { 0,  0, -2}, { 0,  0, -1}, "bottom away from the box" },

	ray_definition{false, { 2,  2,  2}, {-1,  0,  0}, "parallel along x box"     },
	ray_definition{false, { 2,  2,  2}, { 0, -1,  0}, "parallel along y box"     },
	ray_definition{false, { 2,  2,  2}, { 0,  0, -1}, "parallel along z box"     },

	ray_definition{true,  { 0,  2,  1}, { 0, -4, -2}, "through left and right"   },
	ray_definition{true,  { 0,  2,  1}, { 0, -3, -3}, "through bottom and right" },
	ray_definition{true,  { 2,  2,  2}, {-1, -1, -1}, "diagonal through the box" },
	ray_definition{true,  { 0,  0,  0}, { 1,  1,  1}, "from within the box"      },

	ray_definition{true,  { 2,  0,  0}, {-1, -0, -0}, "front of box neg. zero"   }
));
// clang-format on

struct point_definition {
    const bool success;
    const glm::dvec3 point;
    const std::string description;

    point_definition(bool success, const glm::dvec3& point, const std::string& description)
        : success(success), point(point), description(description)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const point_definition& obj)
    {
        return os << obj.description;
    }
};

struct PointBBoxIntersectionTest : public testing::TestWithParam<point_definition> {
    BoundingBox bb;

    PointBBoxIntersectionTest() : bb(glm::dvec3{-1, -1, -1}, glm::dvec3{1, 1, 1}) {}
};

TEST_P(PointBBoxIntersectionTest, testPointIntersection)
{
    const auto p = GetParam();
    const auto success = bb.contains(p.point);

    ASSERT_EQ(success, p.success);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Default, PointBBoxIntersectionTest, testing::Values(
	point_definition{true , { 0,  0,  0}, "Center"             },
						  								       
	point_definition{true , { 1,  0,  0}, "on front side"      },
	point_definition{true , {-1,  0,  0}, "on back side"       },
	point_definition{true , { 0,  1,  0}, "on right side"      },
	point_definition{true , { 0, -1,  0}, "on left side"       },
	point_definition{true , { 0,  0,  1}, "on top side"        },
	point_definition{true , { 0,  0, -1}, "on bottom side"     },

	point_definition{true , { 1,  1,  1}, "on frt corner"      },
	point_definition{true , { 1,  1, -1}, "on frb corner"      },
	point_definition{true , { 1, -1,  1}, "on flt corner"      },
	point_definition{true , { 1, -1, -1}, "on flb corner"      },
	point_definition{true , {-1,  1,  1}, "on brt corner"      },
	point_definition{true , {-1,  1, -1}, "on brb corner"      },
	point_definition{true , {-1, -1,  1}, "on blt corner"      },
	point_definition{true , {-1, -1, -1}, "on blb corner"      },

	point_definition{false, { 2,  0,  0}, "outside front side" },
	point_definition{false, {-2,  0,  0}, "outside back side"  },
	point_definition{false, { 0,  2,  0}, "outside right side" },
	point_definition{false, { 0, -2,  0}, "outside left side"  },
	point_definition{false, { 0,  0,  2}, "outside top side"   },
	point_definition{false, { 0,  0, -2}, "outside bottom side"}

));
// clang-format on

struct bbox_definition {
    const bool success;
    const BoundingBox bbox;
    const std::string description;

    bbox_definition(bool success,
                    const glm::dvec3& min,
                    const glm::dvec3& max,
                    const std::string& description)
        : success(success), bbox{min, max}, description(description)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const bbox_definition& obj)
    {
        return os << obj.description;
    }
};

struct BBoxBBoxIntersectionTest : public testing::TestWithParam<bbox_definition> {
    BoundingBox bb;

    BBoxBBoxIntersectionTest() : bb(glm::dvec3{-1, -1, -1}, glm::dvec3{1, 1, 1}) {}
};

TEST_P(BBoxBBoxIntersectionTest, testRayIntersection)
{
    const auto p = GetParam();
    const auto success = bb.intersect(p.bbox);

    ASSERT_EQ(success, p.success);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Default, BBoxBBoxIntersectionTest, testing::Values(
	bbox_definition{true  , {-1,   -1,   -1  }, { 1,    1,    1  }, "identical"     },
	bbox_definition{true  , {-2,   -2,   -2  }, { 2,    2,    2  }, "around"        },
	bbox_definition{true  , {-0.5, -0.5, -0.5}, { 0.5,  0.5,  0.5}, "inside"        },

	bbox_definition{true  , { 0.5, -0.5, -0.5}, { 1.5,  0.5,  0.5}, "partial front" },
	bbox_definition{true  , {-1.5, -0.5, -0.5}, {-0.5,  0.5,  0.5}, "partial back"  },
	bbox_definition{true  , {-0.5,  0.5, -0.5}, { 0.5,  1.5,  0.5}, "partial right" },
	bbox_definition{true  , {-0.5, -1.5, -0.5}, { 0.5, -0.5,  0.5}, "partial left"  },
	bbox_definition{true  , {-0.5, -0.5,  0.5}, { 0.5,  0.5,  1.5}, "partial top"   },
	bbox_definition{true  , {-0.5, -0.5, -1.5}, { 0.5,  0.5, -0.5}, "partial bottom"},

	bbox_definition{false , { 2,    2,    2  }, { 3,    3,    3  }, "outside"       }

));
// clang-format on

TEST(FlatBBoxTest, testFlatboxFront)
{
    const BoundingBox box({0, -1, -1}, {0, 1, 1});
    const Ray r({-1, 0, 0}, {1, 0, 0});

    ASSERT_TRUE(box.intersect(r));
}

TEST(FlatBBoxTest, testFlatboxRight)
{
    const BoundingBox box({-1, 0, -1}, {1, 0, 1});
    const Ray r({0, -1, 0}, {0, 1, 0});

    ASSERT_TRUE(box.intersect(r));
}

TEST(FlatBBoxTest, testFlatboxTop)
{
    const BoundingBox box({-1, -1, 0}, {1, 1, 0});
    const Ray r({0, 0, -1}, {0, 0, 1});

    ASSERT_TRUE(box.intersect(r));
}
