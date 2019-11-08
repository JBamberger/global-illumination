#pragma once

#include <glm/glm.hpp>

#include "bbox.h"
#include "material.h"
#include "ray.h"

/// A base class for all entities in the scene.
struct Entity {

    constexpr Entity() : material(Material(glm::dvec3(1, 0, 0))) {}
    constexpr Entity(const Material& material) : material(material) {}

    /// Check if a ray intersects the object. The arguments intersect and normal will contain the
    /// point of intersection and its normals.
    virtual bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const = 0;

    /// Returns an axis-aligned bounding box of the entity.
    virtual BoundingBox boundingBox() const = 0;

    glm::dvec3 pos = {0, 0, 0};
    Material material;
};

struct ImplicitSphere : Entity {
    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override {
        // O = ray.origin, D = ray.dir, this->radius = R, this->pos = C
        // (P - C)^2 - R^2 = 0 = (O + Dt - C)^2 - R^2
        // O^2 + 2ODt - 2CDt - 2CO + (Dt)^2 + C^2 - R^2 = 0
        // ( D^2 )t^2 + ( 2DO - 2DC )t + ( O^2 - 2DO + C^2 - R^2  ) = 0
        // ( D^2 )t^2 + ( 2D(O - C) )t + ( (O - C)^2 - R^2  ) = 0
        // x1,x2 = (b -+ sqrt( b^2 - 4ac ))/ 2a
        // q = b^2 - 4ac
        // q = (2D(O-C))^2 - 4 * D^2 ((O-C)^2 - R^2)

        glm::dvec3 L = ray.origin - pos;
        double a = glm::dot(ray.dir, ray.dir);
        double b = 2. * glm::dot(ray.dir, L);
        double c = glm::dot(L, L) - (radius * radius);

        // q < 0 -> no solutions
        // q = 0 -> one solution
        // q > 0 -> two solutions
        double q = b * b - 4 * a * c;
        double x1, x2; // possible solutions
        if (q < 0) {
            return false;
        } else if (q == 0) {
            x1 = x2 = -0.5 * b / a;
        } else {
            double root = glm::sqrt(q);
            x1 = 0.5 * (b - root) / a;
            x2 = 0.5 * (b + root) / a;
        }

        // TODO: select correct solution
        intersect = ray.origin + x1 * ray.dir;
        normal = glm::normalize(intersect - pos);
        return true;
    }

    BoundingBox boundingBox() const override { return {pos - radius, pos + radius}; }

    double radius;
};

// TODO Implement implicit sphere
// TODO Implement implicit triangle

// TODO Implement explicit sphere (triangles)
// TODO Implement explicit quad (triangles)
// TODO Implement explicit cube (triangles)
