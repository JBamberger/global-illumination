#pragma once

#include "entity.h"
#include "triangle.h"

#include <vector>

struct explicit_entity final : entity {
    explicit explicit_entity(std::vector<triangle> faces) : faces(std::move(faces)) {}

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;
    glm::dvec3 get_color_at_intersect(glm::dvec3 intersect) const override;

    /// Creates an explicit sphere from a center and radius by iteratively subdividing each triangle
    /// and projecting it to the implicit sphere. The starting shape is an icosahedron or a
    /// tetrahedron.
    static explicit_entity make_sphere(glm::dvec3 center = {0, 0, 0},
                                       double radius = 1,
                                       int sub_divisions = 2,
                                       bool use_tetrahedron = false);

    /// Creates an explicit quad from the four points.
    static explicit_entity make_quad(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c, glm::dvec3 d);

    /// Creates an explicit axis-aligned cube.
    static explicit_entity make_cube(glm::dvec3 center, double side_length);

    /// Creates an explicit cone.
    static explicit_entity
    make_cone(glm::dvec3 center, glm::dvec3 tip, double radius, size_t slices);

    /// Writes the triangles and vertices in obj format to the output stream.
    std::ostream& write_obj(std::ostream& os);

    std::vector<triangle> faces;
};