#include <ExplicitEntity.h>
#include <ImplicitSphere.h>
#include <ostream>

static const Triangle* last_hit = nullptr;

bool ExplicitEntity::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const
{
    // TODO: check intersection direction (for triangles with only one side)
    // glm::dot(ray.direction, triangle.normal) > 0

    // quickly discard all rays that don't even intersect the bounding box
    if (!boundingBox().intersect(ray)) {
        return false;
    }

    glm::dvec3 i, n;
    auto min = std::numeric_limits<double>::infinity();
    for (const auto& t : faces) {
        if (t.intersect(ray, i, n)) {
            const auto d = glm::distance(i, ray.origin);
            if (d < min) {
                min = d;
                intersect = i;
                normal = n;
                last_hit = &t;
            }
        }
    }
    return min != std::numeric_limits<double>::infinity();
}

BoundingBox ExplicitEntity::boundingBox() const { return bbox; }

glm::dvec3 ExplicitEntity::getColorAtIntersect(glm::dvec3 intersect) const
{
    assert(last_hit != nullptr);

    const auto uv = last_hit->texMapping(intersect);

    return material->getColor(uv);
}

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
