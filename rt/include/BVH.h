#pragma once

#include "Entity.h"
#include <algorithm>
#include <vector>

class BVH : public Entity {
    class Node;
    class LeafNode;
    class InnerNode;

    const size_t cutoff_size_ = 20; // don't split nodes with less elements
    std::unique_ptr<Node> root_;

  public:
    explicit BVH(std::vector<Triangle> faces);

    BoundingBox boundingBox() const override;

    bool intersect(const Ray& ray, Hit& hit) const override;

    void setMaterial(std::shared_ptr<Material> material) override;

  private:
    std::unique_ptr<Node> construct(size_t depth, std::vector<Triangle> faces);
};