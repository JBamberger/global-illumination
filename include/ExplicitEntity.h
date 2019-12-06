#pragma once

#include <Entity.h>
#include <Triangle.h>

#include <memory>
#include <vector>

struct ExplicitEntity final : Entity {
    explicit ExplicitEntity(std::vector<Triangle> faces)
        : faces(std::move(faces)), bbox(computeBBox(this->faces))
    {
    }

    bool intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const override;
    BoundingBox boundingBox() const override;
    glm::dvec3 getColorAtIntersect(glm::dvec3 intersect) const override;

    /// Writes the triangles and vertices in obj format to the output stream.
    std::ostream& writeObj(std::ostream& os);

    std::vector<Triangle> faces;

  private:
    BoundingBox bbox;
    static BoundingBox computeBBox(const std::vector<Triangle>& faces)
    {
        assert(!faces.empty());

        const auto b1 = faces[0].boundingBox();
        auto min = b1.min;
        auto max = b1.max;

        for (const auto& t : faces) {
            const auto bbox = t.boundingBox();
            min = glm::min(min, bbox.min);
            max = glm::max(max, bbox.max);
        }

        return BoundingBox{min, max};
    }
};
