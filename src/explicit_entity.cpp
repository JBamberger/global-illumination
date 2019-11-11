#include "entities.h"

bool explicit_entity::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const
{
    // TODO: check intersection direction
    glm::dvec3 i, n;
    double min = std::numeric_limits<double>::infinity();
    for (const auto& t : faces) {
        if (t.intersect(ray, i, n)) {
            const auto d = glm::distance(i, ray.origin);
            if (d < min) {
                min = d;
                intersect = i;
                normal = n;
            }
        }
    }
    return min != std::numeric_limits<double>::infinity();
}

BoundingBox explicit_entity::boundingBox() const
{
    if (faces.empty()) {
        return BoundingBox{{0, 0, 0}, {0, 0, 0}};
    }
    const auto b1 = faces[0].boundingBox();
    glm::dvec3 min = b1.min;
    glm::dvec3 max = b1.max;

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

explicit_entity explicit_entity::make_quad(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C, glm::dvec3 D)
{
    std::vector<Triangle> faces;
    faces.reserve(2);
    faces.emplace_back(A, B, C);
    faces.emplace_back(C, D, A);
    return explicit_entity(std::move(faces));
}

explicit_entity explicit_entity::make_cube(glm::dvec3 center, glm::dvec3 bottomRightFrontCorner)
{
    glm::dvec3 O = center;
    glm::dvec3 A = bottomRightFrontCorner;
    glm::dvec3 OA = A - O;
    // compute the remaining corner points
    glm::dvec3 B = O + glm::dvec3{OA.x, OA.y, -OA.z};
    glm::dvec3 C = O + glm::dvec3{OA.x, -OA.y, -OA.z};
    glm::dvec3 D = O + glm::dvec3{OA.x, -OA.y, OA.z};

    glm::dvec3 E = O + glm::dvec3{-OA.x, OA.y, OA.z};
    glm::dvec3 F = O + glm::dvec3{-OA.x, OA.y, -OA.z};
    glm::dvec3 G = O + glm::dvec3{-OA.x, -OA.y, -OA.z};
    glm::dvec3 H = O + glm::dvec3{-OA.x, -OA.y, OA.z};

    std::vector<Triangle> faces;
    faces.reserve(12);
    // front
    faces.emplace_back(A, B, C);
    faces.emplace_back(A, C, D);
    // right
    faces.emplace_back(E, F, B);
    faces.emplace_back(E, B, A);
    // back
    faces.emplace_back(H, G, F);
    faces.emplace_back(H, F, E);
    // left
    faces.emplace_back(D, C, G);
    faces.emplace_back(D, G, H);
    // top
    faces.emplace_back(B, F, G);
    faces.emplace_back(B, G, C);
    // bottom
    faces.emplace_back(A, H, E);
    faces.emplace_back(A, D, H);
    return explicit_entity(std::move(faces));
}

/// Projects a point to the given sphere.
inline glm::dvec3 project_to_sphere(const ImplicitSphere& s, const glm::dvec3 p)
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
inline std::vector<Triangle>
perform_subdivision(std::vector<Triangle> ts, const ImplicitSphere& ref, const int sub_divisions)
{
    // Algorithm:
    // for subDivisions do
    //     split every triangle side in half
    //     move the split points to the sphere
    //     form four triangles from them
    // end

    std::vector<Triangle> ts2;
    for (auto d = 0; d < sub_divisions; d++) {
        ts2.clear();
        for (const auto& t : ts) {
            const auto X = project_to_sphere(ref, find_line_center(t.A, t.B));
            const auto Y = project_to_sphere(ref, find_line_center(t.B, t.C));
            const auto Z = project_to_sphere(ref, find_line_center(t.C, t.A));

#if 1
            ts2.emplace_back(t.A, X, Z);
            ts2.emplace_back(X, t.B, Y);
            ts2.emplace_back(Y, t.C, Z);
            ts2.emplace_back(X, Y, Z);
#else
            const auto t1 = Triangle(t.A, X, Z);
            const auto t2 = Triangle(X, t.B, Y);
            const auto t3 = Triangle(Y, t.C, Z);
            const auto t4 = Triangle(X, Y, Z);

            assert(0 >= glm::dot(glm::normalize(t1.normal()), glm::normalize(t2.normal())));
            assert(0 >= glm::dot(glm::normalize(t1.normal()), glm::normalize(t3.normal())));
            assert(0 >= glm::dot(glm::normalize(t1.normal()), glm::normalize(t4.normal())));
            assert(0 >= glm::dot(glm::normalize(t2.normal()), glm::normalize(t3.normal())));
            assert(0 >= glm::dot(glm::normalize(t2.normal()), glm::normalize(t4.normal())));
            assert(0 >= glm::dot(glm::normalize(t3.normal()), glm::normalize(t4.normal())));

            ts2.push_back(t1);
            ts2.push_back(t2);
            ts2.push_back(t3);
            ts2.push_back(t4);
#endif
        }
        std::swap(ts, ts2);
    }
    return ts;
}

/// Creates a tetrahedron where all vertices lie on the given sphere.
inline std::vector<Triangle> get_tetrahedron(const ImplicitSphere& ref)
{
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
}

/// Creates an icosahedron where all vertices lie on the given sphere.
inline std::vector<Triangle> get_icosahedron(const ImplicitSphere& ref)
{
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
}

explicit_entity explicit_entity::make_sphere(const glm::dvec3 center,
                                             const double radius,
                                             const int sub_divisions,
                                             const bool use_tetrahedron)
{
    const auto ref = ImplicitSphere{center, radius};
    const auto initial_shape = use_tetrahedron ? get_tetrahedron(ref) : get_icosahedron(ref);

    auto faces = perform_subdivision(initial_shape, ref, sub_divisions);

    return explicit_entity(std::move(faces));
}

std::ostream& explicit_entity::write_obj(std::ostream& os)
{
    auto vertex = 1;
    for (const auto face : faces) {
        os << "v " << face.A.x << " " << face.A.y << " " << face.A.z << "\n";
        os << "v " << face.B.x << " " << face.B.y << " " << face.B.z << "\n";
        os << "v " << face.C.x << " " << face.C.y << " " << face.C.z << "\n";

        os << "f " << vertex << " " << vertex + 1 << " " << vertex + 2 << std::endl;
        vertex += 3;
    }
    return os;
}
