#include "BVH.h"

class BVH::Node : public Entity {
    BoundingBox bbox_;

  public:
    explicit Node(BoundingBox bbox) : bbox_(bbox) {}
    ~Node() override = default;
    BoundingBox boundingBox() const override { return bbox_; }
};

class BVH::LeafNode : public BVH::Node {

    std::vector<Triangle> content_;

  public:
    explicit LeafNode(std::vector<Triangle> faces)
        : Node(computeBBox(faces)), content_(std::move(faces))
    {
    }
    ~LeafNode() override = default;

    bool intersect(const Ray& ray, Hit& hit) const override
    {

        auto min_dist = std::numeric_limits<double>::max();
        for (const auto& c : content_) {
            Hit tmp_hit;

            // first check the bbox
            // TODO: as long as there are only triangles this check might be obsolete
            if (!c.boundingBox().intersect(ray)) {
                continue;
            }
            // then the actual entity
            if (!c.intersect(ray, tmp_hit)) {
                continue;
            }
            const auto tmp_dist = glm::distance(tmp_hit.pos, ray.origin);
            if (tmp_dist < min_dist) {
                hit = tmp_hit;
                min_dist = tmp_dist;
            }
        }

        return min_dist < std::numeric_limits<double>::max();
    }

    void setMaterial(std::shared_ptr<Material> material) override
    {
        this->material_ = material;
        for (auto& face : content_) {
            face.setMaterial(material);
        }
    }

  private:
    static BoundingBox computeBBox(const std::vector<Triangle>& objects)
    {
        if (objects.empty()) {
            return BoundingBox{{0, 0, 0}, {0, 0, 0}};
        }

        const auto b1 = objects[0].boundingBox();
        auto min = b1.min;
        auto max = b1.max;

        for (const auto& t : objects) {
            const auto bbox = t.boundingBox();
            min = glm::min(min, bbox.min);
            max = glm::max(max, bbox.max);
        }

        return BoundingBox{min, max};
    }
};

class BVH::InnerNode : public BVH::Node {
    std::vector<std::unique_ptr<Node>> children_;

  public:
    explicit InnerNode(std::unique_ptr<Node> lower, std::unique_ptr<Node> upper)
        : Node(BoundingBox::unite(lower->boundingBox(), upper->boundingBox()))
    {
        children_.push_back(std::move(lower));
        children_.push_back(std::move(upper));
    }
    ~InnerNode() override = default;

    bool intersect(const Ray& ray, Hit& hit) const override
    {
        auto min_dist = std::numeric_limits<double>::max();
        for (const auto& c : children_) {
            Hit tmp_hit;
            if (!c->boundingBox().intersect(ray)) {
                continue;
            }
            if (!c->intersect(ray, tmp_hit)) {
                continue;
            }
            const auto tmp_dist = glm::distance(tmp_hit.pos, ray.origin);
            if (tmp_dist < min_dist) {
                hit = tmp_hit;
                min_dist = tmp_dist;
            }
        }

        return min_dist < std::numeric_limits<double>::max();
    }

    void setMaterial(std::shared_ptr<Material> material) override
    {
        this->material_ = material;
        for (auto& n : children_) {
            n->setMaterial(material);
        }
    }
};

BVH::BVH(std::vector<Triangle> faces, size_t cutoffSize) : cutoff_size_(cutoffSize)
{
    root_ = construct(0, std::move(faces));
}

BoundingBox BVH::boundingBox() const { return root_->boundingBox(); }

bool BVH::intersect(const Ray& ray, Hit& hit) const
{
    if (!root_->boundingBox().intersect(ray)) {
        return false;
    }
    return root_->intersect(ray, hit);
}

void BVH::setMaterial(std::shared_ptr<Material> material)
{
    this->material_ = material;
    root_->setMaterial(material);
}

std::unique_ptr<BVH::Node> BVH::construct(size_t depth, std::vector<Triangle> faces)
{
    if (faces.size() < cutoff_size_) {
        return std::make_unique<LeafNode>(std::move(faces));
    }

    const glm::dvec3::length_type cc =
        depth % 3; // determine if x, y or z is used for sorting -> round robin
    const auto comp = [cc](const Triangle& a, const Triangle& b) {
        // compare by center of mass
        return a.A[cc] + a.B[cc] + a.C[cc] < b.A[cc] + b.B[cc] + b.C[cc];
    };
    std::sort(faces.begin(), faces.end(), comp);

    const auto middle = faces.size() / 2;
    std::vector<Triangle> lower(faces.begin(), faces.begin() + middle);
    std::vector<Triangle> upper(faces.begin() + middle, faces.end());

    depth++;
    return std::make_unique<InnerNode>(construct(depth, lower), construct(depth, upper));
}
