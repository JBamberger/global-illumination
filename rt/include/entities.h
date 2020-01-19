#pragma once

#include <Entity.h>
#include <ExplicitEntity.h>

namespace entities {
/// Creates an explicit sphere from a center and radius by iteratively subdividing each triangle
/// and projecting it to the implicit sphere. The starting shape is an icosahedron or a
/// tetrahedron.
std::unique_ptr<ExplicitEntity> makeSphere(glm::dvec3 center = {0, 0, 0},
                                           double radius = 1,
                                           int sub_divisions = 2,
                                           bool use_tetrahedron = false);

/// Creates an explicit quad from the four points.
std::unique_ptr<ExplicitEntity> makeQuad(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c, glm::dvec3 d);

/// Creates an explicit axis-aligned cube.
std::unique_ptr<ExplicitEntity> makeCube(glm::dvec3 center, double side_length);

/// Creates an explicit cone.
std::unique_ptr<ExplicitEntity> makeCone(glm::dvec3 center,
                                         glm::dvec3 tip,
                                         double radius,
                                         size_t slices);
} // namespace entities
