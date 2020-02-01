#include "ObjReader.h"
#include <array>
#include <fstream>
#include <glm/gtx/rotate_vector.hpp>
#include <sstream>
#include <string>
#include <utility>

namespace obj {

ObjContent makeQuad(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c, glm::dvec3 d)
{
    std::vector<Triangle> faces;
    faces.reserve(2);
    faces.emplace_back(a, b, c);
    faces.emplace_back(c, d, a);

    faces.at(0).setTexCoords(glm::dvec2{1, 1}, glm::dvec2{1, 0}, glm::dvec2{0, 0});
    faces.at(1).setTexCoords(glm::dvec2{0, 0}, glm::dvec2{0, 1}, glm::dvec2{1, 1});

    return faces;
}

ObjContent makeOctet(std::array<glm::dvec3, 8> corners)
{

    std::vector<Triangle> faces;
    faces.reserve(12);

    faces.emplace_back(corners[0], corners[1], corners[2]);
    faces.emplace_back(corners[0], corners[2], corners[3]);
    faces.emplace_back(corners[4], corners[1], corners[0]);
    faces.emplace_back(corners[4], corners[5], corners[1]);
    faces.emplace_back(corners[7], corners[5], corners[4]);
    faces.emplace_back(corners[7], corners[6], corners[5]);
    faces.emplace_back(corners[3], corners[6], corners[7]);
    faces.emplace_back(corners[3], corners[2], corners[6]);
    faces.emplace_back(corners[1], corners[5], corners[6]);
    faces.emplace_back(corners[1], corners[6], corners[2]);
    faces.emplace_back(corners[0], corners[7], corners[4]);
    faces.emplace_back(corners[0], corners[3], corners[7]);

    return faces;
}

ObjContent makeCube(glm::dvec3 center, double side_length)
{
    return makeCuboid(center, glm::dvec3(side_length, side_length, side_length));
}

ObjContent makeCuboid(glm::dvec3 center, glm::dvec3 size)
{
    const auto s = size / 2.0;

    std::array<glm::dvec3, 8> corners{
        center + glm::dvec3{s.x, s.y, -s.z},  center + glm::dvec3{s.x, s.y, s.z},
        center + glm::dvec3{s.x, -s.y, s.z},  center + glm::dvec3{s.x, -s.y, -s.z},
        center + glm::dvec3{-s.x, s.y, -s.z}, center + glm::dvec3{-s.x, s.y, s.z},
        center + glm::dvec3{-s.x, -s.y, s.z}, center + glm::dvec3{-s.x, -s.y, -s.z},
    };

    return makeOctet(corners);
}

ObjContent makeSphere(const glm::dvec3 center,
                      const double radius,
                      const int sub_divisions,
                      const bool use_tetrahedron)
{
    /// Projects a point to the given sphere.
    const auto project_to_sphere = [](const Sphere& s, const glm::dvec3 p) {
        const auto r = Ray{s.center, p - s.center};
        Hit hit;
        const auto success = s.intersect(r, hit);

        assert(success);
        return hit.pos;
    };

    /// Computes the point in the middle between two points.
    const auto find_line_center = [](const glm::dvec3 p1, const glm::dvec3 p2) {
        return (p1 + p2) * 0.5;
    };

    /// Subdivision algorithm which splits each triangle side in the middle and projects the new
    /// points to the given sphere. If equilateral the resulting triangles will be equilateral too.
    const auto perform_subdivision =
        [&project_to_sphere, &find_line_center](std::vector<Triangle> ts, const Sphere& ref,
                                                const int sub_divisions) {
            std::vector<Triangle> ts2;
            for (auto d = 0; d < sub_divisions; d++) {
                ts2.clear();
                for (const auto& t : ts) {
                    // split each triangle side and project the center to the sphere
                    const auto x = project_to_sphere(ref, find_line_center(t.A, t.B));
                    const auto y = project_to_sphere(ref, find_line_center(t.B, t.C));
                    const auto z = project_to_sphere(ref, find_line_center(t.C, t.A));

                    // use the new four triangles instead of the single one
                    ts2.emplace_back(t.A, x, z);
                    ts2.emplace_back(x, t.B, y);
                    ts2.emplace_back(y, t.C, z);
                    ts2.emplace_back(x, y, z);
                }
                std::swap(ts, ts2);
            }
            return ts;
        };

    /// Creates a tetrahedron where all vertices lie on the given sphere.
    const auto get_tetrahedron = [&project_to_sphere](const Sphere& ref) {
        std::vector<Triangle> faces;
        faces.reserve(4);

        const auto a = project_to_sphere(ref, ref.center + glm::dvec3{1, 1, 1});
        const auto b = project_to_sphere(ref, ref.center + glm::dvec3{-1, -1, 1});
        const auto c = project_to_sphere(ref, ref.center + glm::dvec3{-1, 1, -1});
        const auto d = project_to_sphere(ref, ref.center + glm::dvec3{1, -1, -1});

        faces.emplace_back(a, c, b);
        faces.emplace_back(a, d, c);
        faces.emplace_back(a, b, d);
        faces.emplace_back(b, c, d);

        return faces;
    };

    /// Creates an icosahedron where all vertices lie on the given sphere.
    const auto get_icosahedron = [&project_to_sphere](const Sphere& ref) {
        std::vector<Triangle> faces;
        faces.reserve(20);

        const auto t = (1.0 + glm::sqrt(5.0)) / 2.0;

        const auto a = project_to_sphere(ref, ref.center + glm::dvec3{-1, t, 0});
        const auto b = project_to_sphere(ref, ref.center + glm::dvec3{1, t, 0});
        const auto c = project_to_sphere(ref, ref.center + glm::dvec3{-1, -t, 0});
        const auto d = project_to_sphere(ref, ref.center + glm::dvec3{1, -t, 0});
        const auto e = project_to_sphere(ref, ref.center + glm::dvec3{0, -1, t});
        const auto f = project_to_sphere(ref, ref.center + glm::dvec3{0, 1, t});
        const auto g = project_to_sphere(ref, ref.center + glm::dvec3{0, -1, -t});
        const auto h = project_to_sphere(ref, ref.center + glm::dvec3{0, 1, -t});
        const auto i = project_to_sphere(ref, ref.center + glm::dvec3{t, 0, -1});
        const auto j = project_to_sphere(ref, ref.center + glm::dvec3{t, 0, 1});
        const auto k = project_to_sphere(ref, ref.center + glm::dvec3{-t, 0, -1});
        const auto l = project_to_sphere(ref, ref.center + glm::dvec3{-t, 0, 1});

        faces.emplace_back(a, l, f);
        faces.emplace_back(a, f, b);
        faces.emplace_back(a, b, h);
        faces.emplace_back(a, h, k);
        faces.emplace_back(a, k, l);
        faces.emplace_back(b, f, j);
        faces.emplace_back(f, l, e);
        faces.emplace_back(l, k, c);
        faces.emplace_back(k, h, g);
        faces.emplace_back(h, b, i);
        faces.emplace_back(d, j, e);
        faces.emplace_back(d, e, c);
        faces.emplace_back(d, c, g);
        faces.emplace_back(d, g, i);
        faces.emplace_back(d, i, j);
        faces.emplace_back(e, j, f);
        faces.emplace_back(c, e, l);
        faces.emplace_back(g, c, k);
        faces.emplace_back(i, g, h);
        faces.emplace_back(j, i, b);

        return faces;
    };

    const auto ref = Sphere{center, radius};
    const auto initial_shape = use_tetrahedron ? get_tetrahedron(ref) : get_icosahedron(ref);

    auto faces = perform_subdivision(initial_shape, ref, sub_divisions);

    return faces;
}

ObjContent makeCone(const glm::dvec3 center,
                    const glm::dvec3 tip,
                    const double radius,
                    const size_t slices)
{
    // prevent figures without volume (the orientation would not be specified completely)
    assert(center != tip);
    assert(radius > 0);
    assert(slices > 2);

    std::vector<Triangle> faces;
    faces.reserve(slices * 2);
    // The cone is built from many wedges which meet in the tip and center of the bottom
    const auto wedge_angle = 2 * glm::pi<double>() / static_cast<double>(slices);
    const auto rotation_axis = glm::normalize(tip - center);

    // find a vector that is not linearly dependent of rotation_axis
    const auto n = glm::abs(glm::dot(rotation_axis, {1, 0, 0})) != 1 ? glm::dvec3{1, 0, 0}
                                                                     : glm::dvec3{0, 1, 0};

    // compute the first point on the bottom plane
    auto last_vec = radius * glm::normalize(glm::cross(rotation_axis, n));
    auto last_border = center + last_vec;

    for (size_t i = 0; i < slices; i++) {

        auto next_vec = glm::rotate(last_vec, wedge_angle, rotation_axis);
        auto next_border = center + next_vec;

        faces.emplace_back(center, next_border, last_border);
        faces.emplace_back(tip, last_border, next_border);

        last_vec = next_vec;
        last_border = next_border;
    }

    return faces;
}

ObjContent readObjFile(const std::string& file)
{
    obj::ObjContent part;
    std::ifstream is(file);
    if (is.is_open()) {
        is >> part;
        is.close();
    } else {
        std::cout << "Could not open file " << file << "." << std::endl;
        throw std::runtime_error("Could not read file.");
    }
    return std::move(part);
}

ObjContent readObjStream(std::istream& is)
{
    ObjContent content;
    readObjStream(is, content);
    return content;
}

void readObjStream(std::istream& is, ObjContent& content)
{
    struct Face {
        glm::ivec3 v;
        glm::ivec3 vt;
        glm::ivec3 vn;
    };

    std::vector<glm::dvec3> vertices;
    std::vector<glm::dvec3> normals;
    std::vector<glm::dvec2> tex_coords;
    std::vector<Face> faces;

    std::string line;
    while (std::getline(is, line)) {
        std::stringstream ls(line);
        std::string kind;
        ls >> kind;
        if (kind == "v") {
            glm::dvec3 v;
            ls >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (kind == "vn") {
            glm::dvec3 vn;
            ls >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        } else if (kind == "vt") {
            glm::dvec2 vt;
            ls >> vt.x >> vt.y;
            tex_coords.push_back(vt);
        } else if (kind == "f") {
            const auto read_one = [&ls](int& v, int& vt, int& vn) {
                ls >> v;
                if (ls.peek() == '/') {
                    static_cast<void>(ls.get());
                    if (ls.peek() == '/') {
                        static_cast<void>(ls.get());
                        ls >> vn;
                    } else {
                        ls >> vt;
                        if (ls.peek() == '/') {
                            static_cast<void>(ls.get());
                            ls >> vn;
                        }
                    }
                }
            };

            Face f{};
            read_one(f.v.x, f.vt.x, f.vn.x);
            read_one(f.v.y, f.vt.y, f.vn.y);
            read_one(f.v.z, f.vt.z, f.vn.z);
            faces.push_back(f);
        } else {
            // s <integer>       -> smoothing 1 - 32
            // s off             -> disable smoothing
            // # <something>     -> comment
            // mtllib <filename> -> material definitions
            // o <string>        -> define object
            // g <string>        -> define group
            // usemtl <string>   -> use material
            // skip line
            continue;
        }
    }

    content.clear();
    for (const auto& f : faces) {
        Triangle t(vertices[f.v.x - 1], vertices[f.v.y - 1], vertices[f.v.z - 1]);
        t.setTexCoords(tex_coords[f.vt.x - 1], tex_coords[f.vt.y - 1], tex_coords[f.vt.z - 1]);
        content.push_back(std::move(t));
    }
    std::cout << "Loaded " << faces.size() << " primitives." << std::endl;
}

std::ostream& operator<<(std::ostream& os, const ObjContent& content)
{
    auto vertex = 1;
    for (const auto& face : content) {
        os << "v " << face.A.x << " " << face.A.y << " " << face.A.z << "\n";
        os << "v " << face.B.x << " " << face.B.y << " " << face.B.z << "\n";
        os << "v " << face.C.x << " " << face.C.y << " " << face.C.z << "\n";

        os << "f " << vertex << " " << vertex + 1 << " " << vertex + 2 << std::endl;
        vertex += 3;
    }
    return os;
}

std::istream& operator>>(std::istream& is, ObjContent& content)
{
    readObjStream(is, content);
    return is;
}

BoundingBox computeBBox(const ObjContent& faces)
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

class Transform::Rotate : public Transform::Step {
    glm::mat3 r_;

  public:
    explicit Rotate(glm::mat3 r) : r_(r) {}

  private:
    void process(Triangle& t) override
    {
        t.A = r_ * t.A;
        t.B = r_ * t.B;
        t.C = r_ * t.C;
    }
};

class Transform::Translate : public Transform::Step {
    glm::dvec3 delta_;

  public:
    explicit Translate(glm::dvec3 delta) : delta_(delta) {}

  private:
    void process(Triangle& t) override
    {
        t.A += delta_;
        t.B += delta_;
        t.C += delta_;
    }
};

class Transform::Scale : public Transform::Step {
    glm::dvec3 scale_;

  public:
    explicit Scale(glm::dvec3 scale) : scale_(scale) {}

  private:
    void process(Triangle& t) override
    {
        t.A *= scale_;
        t.B *= scale_;
        t.C *= scale_;
    }
};

class Transform::Center : public Transform::Step {
    glm::dvec3 mean_;

  public:
    explicit Center() : mean_(0) {}

  private:
    void pre(const ObjContent& content) override
    {
        glm::dvec3 sum(0, 0, 0);
        for (const auto& t : content) {
            sum += (t.A + t.B + t.C) / 3.0;
        }
        sum /= static_cast<double>(content.size());
        mean_ = sum;
    }

    void process(Triangle& t) override
    {
        t.A -= mean_;
        t.B -= mean_;
        t.C -= mean_;
    }
};

Transform& Transform::rotate_x(double angle)
{
    const auto sin_theta = glm::sin(angle);
    const auto cos_theta = glm::cos(angle);

    // clang-format off
    const auto r = glm::transpose(glm::mat3{
        {1, 0, 0},
        {0, cos_theta, sin_theta},
        {0,-sin_theta, cos_theta}
    });
    // clang-format on
    transforms_.push_back(std::unique_ptr<Step>(new Rotate(r)));
    return *this;
}

Transform& Transform::rotate_y(double angle)
{
    const auto sin_theta = glm::sin(angle);
    const auto cos_theta = glm::cos(angle);
    // clang-format off
    const auto r = glm::transpose(glm::mat3{
        { cos_theta, 0, sin_theta},
        {     0, 1,     0},
        {-sin_theta, 0, cos_theta}
    });
    // clang-format on
    transforms_.push_back(std::unique_ptr<Step>(new Rotate(r)));
    return *this;
}

Transform& Transform::rotate_z(double angle)
{
    const auto sin_theta = glm::sin(angle);
    const auto cos_theta = glm::cos(angle);
    // clang-format off
    const auto r = glm::transpose(glm::mat3{
        { cos_theta, sin_theta, 0},
        {-sin_theta, cos_theta, 0},
        {     0,     0, 1}
    });
    // clang-format on
    transforms_.push_back(std::unique_ptr<Step>(new Rotate(r)));
    return *this;
}

Transform& Transform::center()
{
    transforms_.push_back(std::unique_ptr<Step>(new Center()));
    return *this;
}

Transform& Transform::translate(glm::dvec3 delta)
{
    transforms_.push_back(std::unique_ptr<Step>(new Translate(delta)));
    return *this;
}

Transform& Transform::scale(double scale)
{
    transforms_.push_back(std::unique_ptr<Step>(new Scale(glm::dvec3(scale))));
    return *this;
}

Transform& Transform::scale(glm::dvec3 scale)
{
    transforms_.push_back(std::unique_ptr<Step>(new Scale(scale)));
    return *this;
}

ObjContent Transform::apply(ObjContent content) const
{
    for (const auto& t : transforms_) {
        t->pre(content);
    }

    for (auto& triangle : content) {
        for (const auto& t : transforms_) {
            t->process(triangle);
        }
    }

    // finally invalidate all computed properties
    for (auto& face : content) {
        face.invalidate();
    }

    return content;
}

std::unique_ptr<BVH> Transform::to_bvh(ObjContent content) const
{
    return std::make_unique<BVH>(std::move(apply(std::move(content))));
}

std::unique_ptr<BVH> Transform::to_bvh(std::string file) const
{
    return to_bvh(readObjFile(std::move(file)));
}

} // namespace obj