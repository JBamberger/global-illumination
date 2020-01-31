#include <ExplicitEntity.h>
#include <ostream>
#include <sstream>
#include <string>

ExplicitEntity::ExplicitEntity(std::vector<Triangle> faces)
    : faces(std::move(faces)), bbox_(computeBBox(this->faces))
{
}

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

BoundingBox ExplicitEntity::boundingBox() const { return bbox_; }

void ExplicitEntity::invalidate() { bbox_ = computeBBox(faces); }

BoundingBox ExplicitEntity::computeBBox(const std::vector<Triangle>& faces)
{
    if (faces.empty()) {
        return BoundingBox{{0, 0, 0}, {0, 0, 0}};
    }

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

std::ostream& operator<<(std::ostream& os, const ExplicitEntity& entity)
{
    auto vertex = 1;
    for (const auto& face : entity.faces) {
        os << "v " << face.A.x << " " << face.A.y << " " << face.A.z << "\n";
        os << "v " << face.B.x << " " << face.B.y << " " << face.B.z << "\n";
        os << "v " << face.C.x << " " << face.C.y << " " << face.C.z << "\n";

        os << "f " << vertex << " " << vertex + 1 << " " << vertex + 2 << std::endl;
        vertex += 3;
    }
    return os;
}

std::istream& operator>>(std::istream& is, glm::ivec3& vertex)
{
    is >> vertex.x;
    is >> vertex.y;
    is >> vertex.z;
    return is;
}

std::istream& operator>>(std::istream& is, glm::dvec3& vertex)
{
    is >> vertex.x;
    is >> vertex.y;
    is >> vertex.z;
    return is;
}

std::istream& operator>>(std::istream& is, ExplicitEntity& entity)
{
    std::vector<glm::dvec3> vertices;
    std::vector<glm::ivec3> faces;
    std::string line;
    while (std::getline(is, line)) {
        std::stringstream ls(line);
        std::string kind;
        ls >> kind;
        if (kind == "v") {
            glm::dvec3 v;
            ls >> v;
            vertices.push_back(v);
        } else if (kind == "f") {
            glm::ivec3 f;
            ls >> f;
            faces.push_back(f);
        } else {
            // skip line
            continue;
        }
    }

    entity.faces.clear();
    for (const auto& f : faces) {
        entity.faces.emplace_back(vertices[f.x - 1], vertices[f.y - 1], vertices[f.z - 1]);
    }
    entity.invalidate();

    return is;
}
