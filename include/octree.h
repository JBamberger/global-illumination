#pragma once

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "bbox.h"
#include "entities.h"

// #define USE_OCTREE

class octree {
  public:
    octree(const glm::dvec3 min, const glm::dvec3 max) : root_(node({min, max})) {}

    /// Store an entity in the correct position of the octree.
    void push_back(entity* object)
    {
#ifdef USE_OCTREE
        root_.insert(object);
#else
        root_.entities.push_back(object);
#endif
    }

    /// Returns list of entities that have the possibility to be intersected by the ray.
    std::vector<entity*> intersect(const Ray& ray) const
    {
#ifdef USE_OCTREE
        std::vector<entity*> output;
        root_.intersect(ray, output);
        return output;
#else
        return root_.entities;
#endif
    }

  private:
    struct node {
        explicit node(BoundingBox bbox) : bbox(std::move(bbox)) {}

        /// Subdivides the current node into 8 children.
        void partition()
        {
            const auto min = bbox.min;
            const auto center = bbox.center();
            const auto dx = bbox.dx() / 2.0;
            const auto dy = bbox.dy() / 2.0;
            const auto dz = bbox.dz() / 2.0;
            // clang-format off
            children[0] = std::make_unique<node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z},
					        glm::dvec3{center.x, center.y, center.z}});
            children[1] = std::make_unique<node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z + dz},
                            glm::dvec3{center.x, center.y, center.z + dz}});
            children[2] = std::make_unique<node>(
				BoundingBox{glm::dvec3{min.x, min.y + dy, min.z},
                            glm::dvec3{center.x, center.y + dy, center.z}});
            children[3] = std::make_unique<node>(
                BoundingBox{glm::dvec3{min.x, min.y + dy, min.z + dz},
                            glm::dvec3{center.x, center.y + dy, center.z + dz}});
            children[4] = std::make_unique<node>(
				BoundingBox{glm::dvec3{min.x + dx, min.y, min.z},
                            glm::dvec3{center.x + dx, center.y, center.z}});
            children[5] = std::make_unique<node>(
                BoundingBox{glm::dvec3{min.x + dx, min.y, min.z + dz},
                            glm::dvec3{center.x + dx, center.y, center.z + dz}});
            children[6] = std::make_unique<node>(
                BoundingBox{glm::dvec3{min.x + dx, min.y + dy, min.z},
                            glm::dvec3{center.x + dx, center.y + dy, center.z}});
            children[7] = std::make_unique<node>(
                BoundingBox{glm::dvec3{min.x + dx, min.y + dy, min.z + dz},
                            glm::dvec3{center.x + dx, center.y + dy, center.z + dz}});
            // clang-format on

            // insert all entities into the children
            for (auto entity : entities) insert(entity);
            // clear and shrink the own entities vector because it is no longer needed.
            entities.clear();
            entities.shrink_to_fit();
        }

        bool is_leaf() const { return children[0] == nullptr; }

        void insert(entity* e)
        {
            if (is_leaf()) {
                entities.push_back(e);

                if (entities.size() > split_threshold) {
                    partition();
                }
            } else {
                const auto bb = e->boundingBox();

                for (auto& i : children) {
                    if (i->bbox.intersect(bb)) {
                        i->insert(e);
                    }
                }
            }
        }

        void intersect(const Ray& ray, std::vector<entity*>& output) const
        {
            if (!bbox.intersect(ray))
                return;

            if (is_leaf()) {
                output.insert(output.end(), entities.begin(), entities.end());
            } else {
                for (const auto& child : children) { child->intersect(ray, output); }
            }
        }

        BoundingBox bbox;
        std::vector<entity*> entities;
        std::array<std::unique_ptr<node>, 8> children;

        const size_t split_threshold = 8;
    };

    node root_;
};
