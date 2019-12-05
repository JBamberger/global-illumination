#pragma once

#include "entity.h"
#include "triangle.h"

#include <memory>
#include <vector>

struct explicit_entity final : entity {
    explicit explicit_entity(std::vector<triangle> faces)
        : faces(std::move(faces)), bbox(computeBBox(this->faces))
    {
    }

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;
    glm::dvec3 get_color_at_intersect(glm::dvec3 intersect) const override;

    /// Writes the triangles and vertices in obj format to the output stream.
    std::ostream& write_obj(std::ostream& os);

    std::vector<triangle> faces;

  private:
    BoundingBox bbox;
    static BoundingBox computeBBox(const std::vector<triangle>& faces)
    {
        assert(!faces.empty());

        const auto b1 = faces[0].boundingBox();
        auto min = b1.min;
        auto max = b1.max;

        for (const auto& t : faces) {
            const auto bbox = t.boundingBox();
            min = glm::min(min, bbox.min);
            max = glm::max(max, bbox.max);
        }

        return BoundingBox{min, max};
    }
};

namespace entities {
/// Creates an explicit sphere from a center and radius by iteratively subdividing each triangle
/// and projecting it to the implicit sphere. The starting shape is an icosahedron or a
/// tetrahedron.
std::unique_ptr<explicit_entity> make_sphere(glm::dvec3 center = {0, 0, 0},
                                             double radius = 1,
                                             int sub_divisions = 2,
                                             bool use_tetrahedron = false);

/// Creates an explicit quad from the four points.
std::unique_ptr<explicit_entity> make_quad(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c, glm::dvec3 d);

/// Creates an explicit axis-aligned cube.
std::unique_ptr<explicit_entity> make_cube(glm::dvec3 center, double side_length);

/// Creates an explicit cone.
std::unique_ptr<explicit_entity> make_cone(glm::dvec3 center,
                                           glm::dvec3 tip,
                                           double radius,
                                           size_t slices);
} // namespace entities