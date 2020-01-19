#include <ExplicitEntity.h>
#include <ostream>

void ExplicitEntity::setMaterial(std::shared_ptr<Material> material)
{
    this->material_ = material;
    for (auto& face : faces) {
        face.setMaterial(material);
    }
}

bool ExplicitEntity::intersect(const Ray& ray, Hit& hit) const
{
    // TODO: check intersection direction (for triangles with only one side)
    // glm::dot(ray.direction, triangle.normal) > 0

    // quickly discard all rays that don't even intersect the bounding box
    if (!boundingBox().intersect(ray)) {
        return false;
    }

    auto min = std::numeric_limits<double>::infinity();
    Hit tmp_hit;
    for (const auto& t : faces) {
        if (t.intersect(ray, tmp_hit)) {
            const auto d = glm::distance(tmp_hit.pos, ray.origin);
            if (d < min) {
                min = d;
                hit = tmp_hit;
            }
        }
    }
    return min != std::numeric_limits<double>::infinity();
}

BoundingBox ExplicitEntity::boundingBox() const { return bbox; }

std::ostream& ExplicitEntity::writeObj(std::ostream& os)
{
    auto vertex = 1;
    for (const auto& face : faces) {
        os << "v " << face.A.x << " " << face.A.y << " " << face.A.z << "\n";
        os << "v " << face.B.x << " " << face.B.y << " " << face.B.z << "\n";
        os << "v " << face.C.x << " " << face.C.y << " " << face.C.z << "\n";

        os << "f " << vertex << " " << vertex + 1 << " " << vertex + 2 << std::endl;
        vertex += 3;
    }
    return os;
}
