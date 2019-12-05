#include "explicit_entity.h"
#include "implicit_sphere.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

static const triangle* last_hit = nullptr;

bool explicit_entity::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const
{
    // TODO: check intersection direction

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

BoundingBox explicit_entity::boundingBox() const
{
    assert(!faces.empty());

    const auto b1 = faces[0].boundingBox();
    auto min = b1.min;
    auto max = b1.max;

    for (const auto& t : faces) {
        min = glm::min(min, t.A);
        max = glm::max(max, t.A);
        min = glm::min(min, t.B);
        max = glm::max(max, t.B);
        min = glm::min(min, t.C);
        max = glm::max(max, t.C);
    }

    return BoundingBox{min, max};
}

glm::dvec3 explicit_entity::get_color_at_intersect(glm::dvec3 intersect) const
{
    // TODO: implement uv texture mapping
    // simply project the texture up through the object
    // const auto bbox = boundingBox();
    // const auto u = (intersect.x - bbox.min.x) / bbox.dx();
    // const auto v = (intersect.y - bbox.min.y) / bbox.dy();

    // return material->get_color({u, v});
    assert(last_hit != nullptr);
    const auto uv = last_hit->tex_mapping(intersect);
    return material->get_color(uv);
}

std::ostream& explicit_entity::write_obj(std::ostream& os)
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

std::unique_ptr<explicit_entity>
entities::make_quad(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c, glm::dvec3 d)
{
    std::vector<triangle> faces;
    faces.reserve(2);
    faces.emplace_back(a, b, c);
    faces.emplace_back(c, d, a);

    faces.at(0).t1 = {1, 1};
    faces.at(0).t2 = {1, 0};
    faces.at(0).t3 = {0, 0};
    faces.at(1).t1 = {0, 0};
    faces.at(1).t2 = {0, 1};
    faces.at(1).t3 = {1, 1};

    return std::make_unique<explicit_entity>(std::move(faces));
}

std::unique_ptr<explicit_entity> entities::make_cube(glm::dvec3 center, double side_length)
{
    const double l = side_length / 2;

    const auto a = center + glm::dvec3{l, l, -l};
    const auto b = center + glm::dvec3{l, l, l};
    const auto c = center + glm::dvec3{l, -l, l};
    const auto d = center + glm::dvec3{l, -l, -l};
    const auto e = center + glm::dvec3{-l, l, -l};
    const auto f = center + glm::dvec3{-l, l, l};
    const auto g = center + glm::dvec3{-l, -l, l};
    const auto h = center + glm::dvec3{-l, -l, -l};

    std::vector<triangle> faces;
    faces.reserve(12);
    // front
    faces.emplace_back(a, b, c);
    faces.emplace_back(a, c, d);
    // right
    faces.emplace_back(e, b, a);
    faces.emplace_back(e, f, b);
    // back
    faces.emplace_back(h, f, e);
    faces.emplace_back(h, g, f);
    // left
    faces.emplace_back(d, g, h);
    faces.emplace_back(d, c, g);
    // top
    faces.emplace_back(b, f, g);
    faces.emplace_back(b, g, c);
    // bottom
    faces.emplace_back(a, h, e);
    faces.emplace_back(a, d, h);

    return std::make_unique<explicit_entity>(std::move(faces));
}

/// Projects a point to the given sphere.
inline glm::dvec3 project_to_sphere(const implicit_sphere& s, const glm::dvec3 p)
{
    const auto r = Ray{s.center, p - s.center};
    glm::dvec3 i, n;
    const auto success = s.intersect(r, i, n);

    assert(success);
    return i;
}

/// Computes the point in the middle between two points.
inline glm::dvec3 find_line_center(const glm::dvec3 p1, const glm::dvec3 p2)
{
    return (p1 + p2) * 0.5;
}

/// Subdivision algorithm which splits each triangle side in the middle and projects the new points
/// to the given sphere. If equilateral the resulting triangles will be equilateral too.
inline std::vector<triangle>
perform_subdivision(std::vector<triangle> ts, const implicit_sphere& ref, const int sub_divisions)
{
    // Algorithm:
    // for subDivisions do
    //     split every triangle side in half
    //     move the split points to the sphere
    //     form four triangles from them
    // end

    std::vector<triangle> ts2;
    for (auto d = 0; d < sub_divisions; d++) {
        ts2.clear();
        for (const auto& t : ts) {
            const auto x = project_to_sphere(ref, find_line_center(t.A, t.B));
            const auto y = project_to_sphere(ref, find_line_center(t.B, t.C));
            const auto z = project_to_sphere(ref, find_line_center(t.C, t.A));

            ts2.emplace_back(t.A, x, z);
            ts2.emplace_back(x, t.B, y);
            ts2.emplace_back(y, t.C, z);
            ts2.emplace_back(x, y, z);

            // const auto t1 = Triangle(t.A, x, z);
            // const auto t2 = Triangle(x, t.B, y);
            // const auto t3 = Triangle(y, t.C, z);
            // const auto t4 = Triangle(x, y, z);

            // assert(0 >= glm::dot(glm::normalize(t1.normal()), glm::normalize(t2.normal())));
            // assert(0 >= glm::dot(glm::normalize(t1.normal()), glm::normalize(t3.normal())));
            // assert(0 >= glm::dot(glm::normalize(t1.normal()), glm::normalize(t4.normal())));
            // assert(0 >= glm::dot(glm::normalize(t2.normal()), glm::normalize(t3.normal())));
            // assert(0 >= glm::dot(glm::normalize(t2.normal()), glm::normalize(t4.normal())));
            // assert(0 >= glm::dot(glm::normalize(t3.normal()), glm::normalize(t4.normal())));

            // ts2.push_back(t1);
            // ts2.push_back(t2);
            // ts2.push_back(t3);
            // ts2.push_back(t4);
        }
        std::swap(ts, ts2);
    }
    return ts;
}

/// Creates a tetrahedron where all vertices lie on the given sphere.
inline std::vector<triangle> get_tetrahedron(const implicit_sphere& ref)
{
    std::vector<triangle> faces;
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
}

/// Creates an icosahedron where all vertices lie on the given sphere.
inline std::vector<triangle> get_icosahedron(const implicit_sphere& ref)
{
    std::vector<triangle> faces;
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
}

std::unique_ptr<explicit_entity> entities::make_sphere(const glm::dvec3 center,
                                                       const double radius,
                                                       const int sub_divisions,
                                                       const bool use_tetrahedron)
{
    const auto ref = implicit_sphere{center, radius};
    const auto initial_shape = use_tetrahedron ? get_tetrahedron(ref) : get_icosahedron(ref);

    auto faces = perform_subdivision(initial_shape, ref, sub_divisions);

    return std::make_unique<explicit_entity>(std::move(faces));
}

std::unique_ptr<explicit_entity> entities::make_cone(const glm::dvec3 center,
                                                     const glm::dvec3 tip,
                                                     const double radius,
                                                     const size_t slices)
{
    // prevent figures without volume (the orientation would not be specified completely)
    assert(center != tip);
    assert(radius > 0);
    assert(slices > 2);

    std::vector<triangle> faces;
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

    return std::make_unique<explicit_entity>(faces);
}
