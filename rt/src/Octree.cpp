#include <Octree.h>

class Octree::Node : public Hittable {
    BoundingBox bbox_;
    std::vector<Hittable*> entities_;
    std::array<std::unique_ptr<Node>, 8> children_;

    const size_t max_depth_;
    const size_t split_threshold_;

  public:
    explicit Node(BoundingBox bbox, size_t max_depth = 10, size_t split_thresh = 16)
        : bbox_(bbox), max_depth_(max_depth), split_threshold_(split_thresh)
    {
    }

    /// Subdivides the current node into 8 children.
    void partition()
    {
        const auto min = bbox_.min;
        const auto c = (bbox_.max + bbox_.min) / 2.0;
        const auto d = (bbox_.max - bbox_.min) / 2.0;
        // clang-format off
            children_[0] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z},
					        glm::dvec3{c.x, c.y, c.z}});
            children_[1] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y, min.z + d.z},
                            glm::dvec3{c.x, c.y, c.z + d.z}});
            children_[2] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x, min.y + d.y, min.z},
                            glm::dvec3{c.x, c.y + d.y, c.z}});
            children_[3] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x, min.y + d.y, min.z + d.z},
                            glm::dvec3{c.x, c.y + d.y, c.z + d.z}});
            children_[4] = std::make_unique<Node>(
				BoundingBox{glm::dvec3{min.x + d.x, min.y, min.z},
                            glm::dvec3{c.x + d.x, c.y, c.z}});
            children_[5] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + d.x, min.y, min.z + d.z},
                            glm::dvec3{c.x + d.x, c.y, c.z + d.z}});
            children_[6] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + d.x, min.y + d.y, min.z},
                            glm::dvec3{c.x + d.x, c.y + d.y, c.z}});
            children_[7] = std::make_unique<Node>(
                BoundingBox{glm::dvec3{min.x + d.x, min.y + d.y, min.z + d.z},
                            glm::dvec3{c.x + d.x, c.y + d.y, c.z + d.z}});
        // clang-format on

        // insert all entities into the children
        std::vector<Hittable*> tmp;
        for (auto entity : entities_) {
            const auto bb = entity->boundingBox();

            Node* receiver = nullptr;
            for (auto& i : children_) {
                if (i->bbox_.intersect(bb)) {
                    if (receiver != nullptr) {
                        receiver = nullptr;
                        break;
                    }
                    receiver = i.get();
                }
            }
            if (receiver != nullptr) {
                receiver->entities_.push_back(entity);
            } else {
                tmp.push_back(entity);
            }
        }
        // clear and shrink the own entities vector because it is no longer needed.
        entities_.clear();
        entities_.insert(entities_.end(), tmp.begin(), tmp.end());
        entities_.shrink_to_fit();
    }

    bool isLeaf() const { return children_[0] == nullptr; }

    void insert(Hittable* e, const size_t depth)
    {
        if (isLeaf()) {
            entities_.push_back(e);

            if (depth < max_depth_ && entities_.size() > split_threshold_) {
                partition();
            }
        } else {
            const auto bb = e->boundingBox();

            Node* receiver = nullptr;
            for (auto& i : children_) {
                if (i->bbox_.intersect(bb)) {
                    if (receiver != nullptr) {
                        receiver = nullptr;
                        break;
                    }
                    receiver = i.get();
                }
            }
            if (receiver != nullptr) {
                receiver->entities_.push_back(e);
            } else {
                entities_.push_back(e);
            }
        }
    }

    void intersect(const Ray& ray, std::vector<const Hittable*>& output) const
    {
        if (!bbox_.intersect(ray))
            return;

        output.insert(output.end(), entities_.begin(), entities_.end());

        if (!isLeaf()) {
            for (const auto& child : children_) {
                child->intersect(ray, output);
            }
        }
    }

    size_t size() const
    {
        assert(isLeaf());

        return entities_.size();
    }

    bool intersect(const Ray& ray, Hit& hit) const override
    {
        auto min_dist = std::numeric_limits<double>::max();
        for (const auto& e : entities_) {
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
            for (const auto& c : children_) {
                if (!c->bbox_.intersect(ray)) {
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

    BoundingBox boundingBox() const override { return bbox_; }

    friend std::ostream& operator<<(std::ostream& o, const Node& n)
    {
        if (n.isLeaf()) {
            o << "(" << n.size() << ")";
        } else {
            o << "(" << n.size() << "\n";
            for (const auto& child : n.children_) {
                o << *child << "\n";
            }
            o << ")";
        }
        return o;
    }
};

Octree::Octree(const glm::dvec3 min, const glm::dvec3 max)
    : root_(std::make_unique<Node>(BoundingBox(min, max)))
{
}

void Octree::insert(Hittable* object) const { root_->insert(object, 0); }

bool Octree::intersect(const Ray& ray, Hit& hit) const { return root_->intersect(ray, hit); }

BoundingBox Octree::boundingBox() const { return root_->boundingBox(); }

std::ostream& operator<<(std::ostream& o, const Octree& t) { return o << "{" << *t.root_ << "}"; }

void Octree::clear() { root_ = std::make_unique<Node>(root_->boundingBox()); }
