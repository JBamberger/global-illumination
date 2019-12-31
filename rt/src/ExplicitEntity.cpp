#include <ExplicitEntity.h>
#include <ImplicitSphere.h>
#include <ostream>

void ExplicitEntity::setMaterial(std::shared_ptr<Material> material)
{
    this->material = material;
    for (auto& face : faces) { face.material = material; }
}

const Entity* ExplicitEntity::intersect(const Ray& ray,
                                        glm::dvec3& intersect,
                                        glm::dvec3& normal) const
{
    // TODO: check intersection direction (for triangles with only one side)
    // glm::dot(ray.direction, triangle.normal) > 0

    // quickly discard all rays that don't even intersect the bounding box
    if (!boundingBox().intersect(ray)) {
        return nullptr;
    }

    const Entity* last_hit = nullptr;
    glm::dvec3 i, n;
    auto min = std::numeric_limits<double>::infinity();
    for (const auto& t : faces) {
        const Entity* e = t.intersect(ray, i, n);
        if (e) {
            const auto d = glm::distance(i, ray.origin);
            if (d < min) {
                min = d;
                intersect = i;
                normal = n;
                last_hit = e;
            }
        }
    }
    return last_hit;
}

BoundingBox ExplicitEntity::boundingBox() const { return bbox; }

glm::dvec3 ExplicitEntity::getColorAtIntersect(glm::dvec3 intersect) const
{
    throw std::exception();
    // assert(last_hit != nullptr);

    // const auto uv = last_hit->texMapping(intersect);

    // return material->getColor(uv);
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
