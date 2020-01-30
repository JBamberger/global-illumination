#pragma once

#include <Entity.h>
#include <ExplicitEntity.h>
#include <array>

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

/// Creates an explict cuboid
std::unique_ptr<ExplicitEntity> makeCuboid(glm::dvec3 center, glm::dvec3 size);

/// Creates an explict cuboid
std::unique_ptr<ExplicitEntity> makeOctet(std::array<glm::dvec3, 8> corners);

/// Creates an explicit cone.
std::unique_ptr<ExplicitEntity> makeCone(glm::dvec3 center,
                                         glm::dvec3 tip,
                                         double radius,
                                         size_t slices);

std::unique_ptr<ExplicitEntity> rotate_z(std::unique_ptr<ExplicitEntity> entity, double angle);
std::unique_ptr<ExplicitEntity> translate(std::unique_ptr<ExplicitEntity> entity,
                                          glm::dvec3 offset);

} // namespace entities
