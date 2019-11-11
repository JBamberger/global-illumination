#pragma once

struct explicit_entity final : Entity {
    explicit explicit_entity(std::vector<Triangle> faces) : faces(std::move(faces)) {}

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;

    /// Creates an explicit sphere from a center and radius by iteratively subdividing each triangle
    /// and projecting it to the implicit sphere. The starting shape is an icosahedron or a
    /// tetrahedron.
    static explicit_entity make_sphere(glm::dvec3 center = {0, 0, 0},
                                       double radius = 1,
                                       int sub_divisions = 2,
                                       bool use_tetrahedron = false);

    /// Creates an explicit quad from the four points.
    static explicit_entity make_quad(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C, glm::dvec3 D);

    /// Creates an explicit cube entity.
    static explicit_entity make_cube(glm::dvec3 center, glm::dvec3 bottomRightFrontCorner);

    /// Writes the triangles and vertices in obj format to the output stream.
    std::ostream& write_obj(std::ostream& os);

    std::vector<Triangle> faces;
};