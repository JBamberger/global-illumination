#pragma once

#include <glm/glm.hpp>

#include "bbox.h"
#include "material.h"
#include "ray.h"
#include <vector>

/// A base class for all entities in the scene.
struct Entity {

    explicit constexpr Entity() : material(Material(glm::dvec3(1, 0, 0))) {}
    explicit constexpr Entity(const Material& material) : material(material) {}
    virtual ~Entity() = default;

    /// Check if a ray intersects the object. The arguments intersect and normal will contain the
    /// point of intersection and its normals.
    virtual bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const = 0;

    /// Returns an axis-aligned bounding box of the entity.
    virtual BoundingBox boundingBox() const = 0;

    Material material;
};

struct ImplicitSphere final : Entity {
    ImplicitSphere() : ImplicitSphere({0, 0, 0}, 1) {}
    ImplicitSphere(glm::dvec3 center, double radius);
    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;

    glm::dvec3 center = {0, 0, 0};
    double radius = 0;
};

struct Triangle final : Entity {
    Triangle();
    Triangle(const Material& material);
    Triangle(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C);

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;

    inline glm::dvec3 normal() const { return glm::normalize(glm::cross(B - A, C - A)); }

    glm::dvec3 A = {0, 0, 0};
    glm::dvec3 B = {0, 0, 0};
    glm::dvec3 C = {0, 0, 0};
};

struct ExplicitEntity : Entity {
    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;

    std::vector<Triangle> faces;
};

struct Quad final : ExplicitEntity {
    Quad() = delete;
    Quad(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C, glm::dvec3 D);
};

struct Cube final : ExplicitEntity {
    Cube() = delete;
    Cube(glm::dvec3 center, glm::dvec3 bottomRightFrontCorner);
};

struct ExplicitSphere final : ExplicitEntity {
    explicit ExplicitSphere() : ExplicitSphere({0, 0, 0}, 1, 3) {}
    explicit ExplicitSphere(glm::dvec3 center, double radius, int subDivisions);
};

// TODO Implement implicit sphere
// TODO Implement implicit triangle

// TODO Implement explicit sphere (triangles)
// TODO Implement explicit quad (triangles)
// TODO Implement explicit cube (triangles)
