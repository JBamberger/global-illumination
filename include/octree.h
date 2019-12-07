#pragma once

#include <array>
#include <memory>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "bbox.h"
#include "entities.h"

#define USE_OCTREE

class Octree {
  public:
    Octree(const glm::dvec3 min, const glm::dvec3 max) : root_(Node({min, max})) {}

    BoundingBox bounds() const { return root_.bbox; }

    /// Store an entity in the correct position of the octree.
    void pushBack(Entity* object)
    {
#ifdef USE_OCTREE
        /*auto ee = dynamic_cast<ExplicitEntity*>(object);
        if (ee) {
            for (auto& face : ee->faces) { root_.insert(face, 0); }
        } else {
            root_.insert(object, 0);
        }*/
        root_.insert(object, 0);
#else
        root_.entities.push_back(object);
#endif
    }

    /// Returns list of entities that have the possibility to be intersected by the ray.
    std::vector<Entity*> intersect(const Ray& ray) const
    {
#ifdef USE_OCTREE
        std::vector<Entity*> output;
        root_.intersect(ray, output);
        return output;
#else
        return root_.entities;
#endif
    }

    Entity* closestIntersection(const Ray& ray, glm::dvec3& inter, glm::dvec3& normal) const
    {
        Entity* min_ent = nullptr;
        auto min = std::numeric_limits<double>::infinity();
        glm::dvec3 i, n;

        const auto entities = intersect(ray);
        for (auto entity : entities) {
            if (entity->intersect(ray, i, n)) {
                const auto dist = glm::distance(ray.origin, i);
                if (dist < min) {
                    min = dist;
                    normal = n;
                    inter = i;
                    min_ent = entity;
                }
            }
        }
        return min_ent;
    }

    bool isBlocked(const Ray& ray) const
    {
        auto blocked = false;
        glm::dvec3 i, n;

        const auto entities = intersect(ray);
        for (auto entity : entities) {
            if (entity->intersect(ray, i, n)) {
                blocked = true;
                break;
            }
        }
        return blocked;
    }

    friend std::ostream& operator<<(std::ostream& o, const Octree& t)
    {
        return o << "{" << t.root_ << "}";
    }

  private:
    struct Node {
        explicit Node(BoundingBox bbox) : bbox(std::move(bbox)) {}

        /// Subdivides the current node into 8 children.
        void partition()
        {
            const auto min = bbox.min;
            const auto center = bbox.center();
            const auto dx = bbox.dx() / 2.0;
            const auto dy = bbox.dy() / 2.0;
            const auto dz = bbox.dz() / 2.0;
            // clang-format off
            children[0] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z},
					        glm::dvec3{center.x, center.y, center.z}});
            children[1] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z + dz},
                            glm::dvec3{center.x, center.y, center.z + dz}});
            children[2] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y + dy, min.z},
                            glm::dvec3{center.x, center.y + dy, center.z}});
            children[3] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x, min.y + dy, min.z + dz},
                            glm::dvec3{center.x, center.y + dy, center.z + dz}});
            children[4] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x + dx, min.y, min.z},
                            glm::dvec3{center.x + dx, center.y, center.z}});
            children[5] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + dx, min.y, min.z + dz},
                            glm::dvec3{center.x + dx, center.y, center.z + dz}});
            children[6] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + dx, min.y + dy, min.z},
                            glm::dvec3{center.x + dx, center.y + dy, center.z}});
            children[7] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + dx, min.y + dy, min.z + dz},
                            glm::dvec3{center.x + dx, center.y + dy, center.z + dz}});
            // clang-format on

            // insert all entities into the children
            std::vector<Entity*> tmp;
            for (auto entity : entities) {
                const auto bb = entity->boundingBox();

                Node* receiver = nullptr;
                for (auto& i : children) {
                    if (i->bbox.intersect(bb)) {
                        if (receiver != nullptr) {
                            receiver = nullptr;
                            break;
                        }
                        receiver = i.get();
                    }
                }
                if (receiver != nullptr) {
                    receiver->entities.push_back(entity);
                } else {
                    tmp.push_back(entity);
                }
            }
            // clear and shrink the own entities vector because it is no longer needed.
            entities.clear();
            entities.insert(entities.end(), tmp.begin(), tmp.end());
            entities.shrink_to_fit();
        }

        bool isLeaf() const { return children[0] == nullptr; }

        void insert(Entity* e, const size_t depth)
        {
            if (isLeaf()) {
                entities.push_back(e);

                if (depth < split_threshold && entities.size() > split_threshold) {
                    partition();
                }
            } else {
                const auto bb = e->boundingBox();

                for (auto& i : children) {
                    if (i->bbox.intersect(bb)) {
                        i->insert(e, depth + 1);
                    }
                }
            }
        }

        void intersect(const Ray& ray, std::vector<Entity*>& output) const
        {
            if (!bbox.intersect(ray))
                return;

            output.insert(output.end(), entities.begin(), entities.end());

            if (!isLeaf()) {
                for (const auto& child : children) { child->intersect(ray, output); }
            }
        }

        size_t size() const
        {
            assert(isLeaf());

            return entities.size();
        }

        friend std::ostream& operator<<(std::ostream& o, const Node& n)
        {
            if (n.isLeaf()) {
                o << "(" << n.size() << ")";
            } else {
                o << "(" << n.size() << "\n";
                for (const auto& child : n.children) { o << *child << "\n"; }
                o << ")";
            }
            return o;
        }

        BoundingBox bbox;
        std::vector<Entity*> entities;
        std::array<std::unique_ptr<Node>, 8> children;

        const size_t split_threshold = 16;
        const size_t max_depth = 10;
    };

    Node root_;
};
