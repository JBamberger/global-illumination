#pragma once

#include <Entity.h>

#include <memory>
#include <ostream>
#include <vector>

struct ExplicitEntity final : Entity {
    explicit ExplicitEntity(std::vector<Triangle> faces);

    void setMaterial(std::shared_ptr<Material> material) override;

    bool intersect(const Ray& ray, Hit& hit) const override;

    BoundingBox boundingBox() const override;

    std::vector<Triangle> faces;

  private:
    BoundingBox bbox_;
};
