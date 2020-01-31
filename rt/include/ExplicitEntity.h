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

    void invalidate();

    std::vector<Triangle> faces;

    friend std::ostream& operator<<(std::ostream& os, const ExplicitEntity& entity);

    friend std::istream& operator>>(std::istream& is, ExplicitEntity& entity);

  private:
    BoundingBox bbox_;

    static BoundingBox computeBBox(const std::vector<Triangle>& faces);
};
