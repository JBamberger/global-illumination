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
    void pushBack(Hittable* object) const;

    /// Returns list of entities that have the possibility to be intersected by the ray.
    std::vector<const Hittable*> intersect(const Ray& ray) const;

    /// Returns the entity that is intersects the ray closest to the rays origin.
    bool closestIntersection(const Ray& ray, Hit& hit) const;

    /// Checks if the ray is blocked by an object
    bool isBlocked(const Ray& ray, const glm::dvec3& light) const;

    friend std::ostream& operator<<(std::ostream& o, const Octree& t);
};
