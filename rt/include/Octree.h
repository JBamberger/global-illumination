#pragma once

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include <BoundingBox.h>
#include <entities.h>

#define USE_OCTREE

class Octree {
    struct Node;
    std::unique_ptr<Node> root_;

  public:
    Octree(glm::dvec3 min, glm::dvec3 max);
    ~Octree();

    /// returns the boundaries of the octree
    BoundingBox bounds() const;

    /// Store an entity in the correct position of the octree.
    void pushBack(Entity* object) const;

    /// Returns list of entities that have the possibility to be intersected by the ray.
    std::vector<const Entity*> intersect(const Ray& ray) const;

    /// Returns the entity that is intersects the ray closest to the rays origin.
    const Entity* closestIntersection(const Ray& ray, glm::dvec3& inter, glm::dvec3& normal) const;

    /// Checks if the ray is blocked by an object
    /// TODO: This should also check that the intersection appears between light and object, not
    /// behind the light
    bool isBlocked(const Ray& ray) const;

    friend std::ostream& operator<<(std::ostream& o, const Octree& t);
};
