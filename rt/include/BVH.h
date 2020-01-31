#pragma once

#include "Entity.h"
#include <algorithm>
#include <vector>
class BVH : public Hittable {

    class Node : public Hittable {
        BoundingBox bbox_;

      public:
        explicit Node(BoundingBox bbox) : bbox_(bbox) {}
        ~Node() override = default;
        BoundingBox boundingBox() const override { return bbox_; }
    };

    class LeafNode : public Node {

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
                const auto tmp_dist = glm::distance(hit.pos, ray.origin);
                if (tmp_dist < min_dist) {
                    hit = tmp_hit;
                    min_dist = tmp_dist;
                }
            }

            return min_dist < std::numeric_limits<double>::max();
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

    class InnerNode : public Node {
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
                if (c->intersect(ray, tmp_hit)) {
                    const auto tmp_dist = glm::distance(hit.pos, ray.origin);
                    if (tmp_dist < min_dist) {
                        hit = tmp_hit;
                        min_dist = tmp_dist;
                    }
                }
            }

            return min_dist < std::numeric_limits<double>::max();
        }
    };

    const size_t cutoff_size_ = 20; // don't split nodes with less elements
    std::unique_ptr<Node> root_;

  public:
    explicit BVH(std::vector<Triangle> faces) { root_ = construct(0, std::move(faces)); }

    BoundingBox boundingBox() const override { return root_->boundingBox(); }

    bool intersect(const Ray& ray, Hit& hit) const override { return root_->intersect(ray, hit); }

  private:
    std::unique_ptr<Node> construct(size_t depth, std::vector<Triangle> faces)
    {
        if (faces.size() < cutoff_size_) {
            return std::make_unique<LeafNode>(std::move(faces));
        }

        const auto cc = depth % 3; // determine if x, y or z is used for sorting -> round robin
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
};