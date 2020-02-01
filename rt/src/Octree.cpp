#include <Octree.h>

struct Octree::Node : Hittable {
    explicit Node(BoundingBox bbox) : bbox(bbox) {}

    /// Subdivides the current node into 8 children.
    void partition()
    {
        const auto min = bbox.min;
        const auto c = (bbox.max + bbox.min) / 2.0;
        const auto d = (bbox.max - bbox.min) / 2.0;
        // clang-format off
            children[0] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z},
					        glm::dvec3{c.x, c.y, c.z}});
            children[1] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z + d.z},
                            glm::dvec3{c.x, c.y, c.z + d.z}});
            children[2] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y + d.y, min.z},
                            glm::dvec3{c.x, c.y + d.y, c.z}});
            children[3] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x, min.y + d.y, min.z + d.z},
                            glm::dvec3{c.x, c.y + d.y, c.z + d.z}});
            children[4] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x + d.x, min.y, min.z},
                            glm::dvec3{c.x + d.x, c.y, c.z}});
            children[5] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + d.x, min.y, min.z + d.z},
                            glm::dvec3{c.x + d.x, c.y, c.z + d.z}});
            children[6] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + d.x, min.y + d.y, min.z},
                            glm::dvec3{c.x + d.x, c.y + d.y, c.z}});
            children[7] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + d.x, min.y + d.y, min.z + d.z},
                            glm::dvec3{c.x + d.x, c.y + d.y, c.z + d.z}});
        // clang-format on

        // insert all entities into the children
        std::vector<Hittable*> tmp;
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

    void insert(Hittable* e, const size_t depth)
    {
        if (isLeaf()) {
            entities.push_back(e);

            if (depth < split_threshold && entities.size() > split_threshold) {
                partition();
            }
        } else {
            const auto bb = e->boundingBox();

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
                receiver->entities.push_back(e);
            } else {
                entities.push_back(e);
            }
        }
    }

    void intersect(const Ray& ray, std::vector<const Hittable*>& output) const
    {
        if (!bbox.intersect(ray))
            return;

        output.insert(output.end(), entities.begin(), entities.end());

        if (!isLeaf()) {
            for (const auto& child : children) {
                child->intersect(ray, output);
            }
        }
    }

    size_t size() const
    {
        assert(isLeaf());

        return entities.size();
    }

    bool intersect(const Ray& ray, Hit& hit) const override
    {
        auto min_dist = std::numeric_limits<double>::max();
        for (const auto& e : entities) {
            Hit tmp_hit;
            if (!e->boundingBox().intersect(ray)) {
                continue;
            }
            if (!e->intersect(ray, tmp_hit)) {
                continue;
            }
            const auto tmp_dist = glm::distance(tmp_hit.pos, ray.origin);
            if (tmp_dist < min_dist) {
                hit = tmp_hit;
                min_dist = tmp_dist;
            }
        }

        if (!isLeaf()) {
            for (const auto& c : children) {
                if (!c->bbox.intersect(ray)) {
                    continue;
                }
                Hit tmp_hit;
                if (!c->intersect(ray, tmp_hit)) {
                    continue;
                }
                const auto tmp_dist = glm::distance(tmp_hit.pos, ray.origin);
                if (tmp_dist < min_dist) {
                    hit = tmp_hit;
                    min_dist = tmp_dist;
                }
            }
        }

        return min_dist < std::numeric_limits<double>::max();
    }

    BoundingBox boundingBox() const override { return bbox; }

    friend std::ostream& operator<<(std::ostream& o, const Node& n)
    {
        if (n.isLeaf()) {
            o << "(" << n.size() << ")";
        } else {
            o << "(" << n.size() << "\n";
            for (const auto& child : n.children) {
                o << *child << "\n";
            }
            o << ")";
        }
        return o;
    }

    BoundingBox bbox;
    std::vector<Hittable*> entities;
    std::array<std::unique_ptr<Node>, 8> children;

    const size_t split_threshold = 16;
    const size_t max_depth = 10;
};

Octree::Octree(const glm::dvec3 min, const glm::dvec3 max)
    : root_(std::make_unique<Node>(BoundingBox(min, max)))
{
}

BoundingBox Octree::bounds() const { return root_->bbox; }

/// Store an entity in the correct position of the octree.
void Octree::pushBack(Hittable* object) const { root_->insert(object, 0); }

/// Returns list of entities that have the possibility to be intersected by the ray.
std::vector<const Hittable*> Octree::intersect(const Ray& ray) const
{
    std::vector<const Hittable*> output;
    root_->intersect(ray, output);
    return output;
}

bool Octree::closestIntersection(const Ray& ray, Hit& hit) const
{
    auto min = std::numeric_limits<double>::infinity();
    Hit tmp_hit;

    const auto entities = intersect(ray);
    for (auto entity : entities) {
        if (entity->intersect(ray, tmp_hit)) {
            const auto dist = glm::distance(ray.origin, tmp_hit.pos);
            if (dist < min) {
                min = dist;
                hit = tmp_hit;
            }
        }
    }
    return min != std::numeric_limits<double>::infinity();
}

bool Octree::isBlocked(const Ray& ray, const glm::dvec3& light) const
{
    auto blocked = false;
    const auto light_dist = glm::length(light - ray.origin);
    const auto entities = intersect(ray);
    for (auto entity : entities) {
        Hit hit;
        if (entity->intersect(ray, hit)) {
            // check that the entity is not behind the light source
            const auto entity_dist = glm::length(hit.pos - ray.origin);
            if (light_dist > entity_dist) {
                blocked = true;
                break;
            }
        }
    }
    return blocked;
}

std::ostream& operator<<(std::ostream& o, const Octree& t) { return o << "{" << *t.root_ << "}"; }

bool Octree::intersect(const Ray& ray, Hit& hit) const { return root_->intersect(ray, hit); }

BoundingBox Octree::boundingBox() const { return root_->bbox; }
