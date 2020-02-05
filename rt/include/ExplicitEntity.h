#pragma once

#include "Entity.h"
#include <memory>
#include <ostream>
#include <vector>

/**
 * This class is a simple compound entity which does not use any form of hierarchic optimization.
 * The elements are stored in a vector and checked all at once.
 */
struct ExplicitEntity final : Entity {
    explicit ExplicitEntity(std::vector<Triangle> faces);

    void setMaterial(std::shared_ptr<Material> material) override;

    [[nodiscard]] bool intersect(const Ray& ray, Hit& hit) const override;

    [[nodiscard]] BoundingBox boundingBox() const override;

  private:
    std::vector<Triangle> faces_;
    BoundingBox bbox_;
};
