#pragma once

#include "bbox.h"
#include "material.h"
#include "ray.h"
#include "simple_material.h"
#include <glm/glm.hpp>
#include <memory>
#include <utility>

/// A base class for all entities in the scene.
struct entity {

    explicit entity() : material(std::make_shared<simple_material>(glm::dvec3{1, 0, 0})) {}
    explicit entity(std::shared_ptr<Material> material) : material(std::move(material)) {}
    virtual ~entity() = default;

    /// Check if a ray intersects the object. The arguments intersect and normal will contain the
    /// point of intersection and its normals.
    virtual bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const = 0;

    /// Returns an axis-aligned bounding box of the entity.
    virtual BoundingBox boundingBox() const = 0;

    // TODO: make material private
    // TODO: add function to obtain material for an intersection location
    // TODO: every implementor must provide the correct material. This allows to use different
    // TODO: projections / uv coordinates

    std::shared_ptr<Material> material;
};
