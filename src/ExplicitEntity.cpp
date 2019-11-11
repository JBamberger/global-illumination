#include "entities.h"

bool ExplicitEntity::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const
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

BoundingBox ExplicitEntity::boundingBox() const
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

ExplicitEntity ExplicitEntity::make_quad(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C, glm::dvec3 D)
{
    std::vector<Triangle> faces;
    faces.reserve(2);
    faces.emplace_back(A, B, C);
    faces.emplace_back(C, D, A);
    return ExplicitEntity(std::move(faces));
}

ExplicitEntity ExplicitEntity::make_cube(glm::dvec3 center, glm::dvec3 bottomRightFrontCorner)
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
    return ExplicitEntity(std::move(faces));
}

// projects a point to the given sphere
inline glm::dvec3 projectToSphere(const ImplicitSphere& s, glm::dvec3 P)
{
    const auto r = Ray{s.center, P - s.center};
    glm::dvec3 i, n;
    const auto success = s.intersect(r, i, n);

    assert(success);
    return i;
}

/// computes the middle point between two points and projects it to the sphere
inline glm::dvec3 findCenter(const ImplicitSphere& s, glm::dvec3 p1, glm::dvec3 p2)
{
    return projectToSphere(s, (p1 + p2) * 0.5);
}

ExplicitEntity ExplicitEntity::make_sphere(glm::dvec3 center, double radius, int subDivisions)
{
    // Algorithm:
    // start with tetrahedron
    // for division
    //     split every triangle side in half
    //     move the split points to the sphere
    //     form four triangles from them
    // end for

    const auto ref = ImplicitSphere{center, radius};
    const auto size = 0;
    std::vector<Triangle> ts;
    ts.reserve(size);
    std::vector<Triangle> ts2;
    ts2.reserve(size);

    // build tetrahedron
    // const auto P1 = projectToSphere(ref, center + glm::dvec3{1, 1, 1});
    // const auto P2 = projectToSphere(ref, center + glm::dvec3{-1, -1, 1});
    // const auto P3 = projectToSphere(ref, center + glm::dvec3{-1, 1, -1});
    // const auto P4 = projectToSphere(ref, center + glm::dvec3{1, -1, -1});
    // ts.emplace_back(P1, P3, P2);
    // ts.emplace_back(P1, P4, P3);
    // ts.emplace_back(P1, P2, P4);
    // ts.emplace_back(P2, P3, P4);
    //
    const auto t = (1.0 + glm::sqrt(5.0)) / 2.0;

    const auto P0 = projectToSphere(ref, center + glm::dvec3{-1, t, 0});
    const auto P1 = projectToSphere(ref, center + glm::dvec3{1, t, 0});
    const auto P2 = projectToSphere(ref, center + glm::dvec3{-1, -t, 0});
    const auto P3 = projectToSphere(ref, center + glm::dvec3{1, -t, 0});
    const auto P4 = projectToSphere(ref, center + glm::dvec3{0, -1, t});
    const auto P5 = projectToSphere(ref, center + glm::dvec3{0, 1, t});
    const auto P6 = projectToSphere(ref, center + glm::dvec3{0, -1, -t});
    const auto P7 = projectToSphere(ref, center + glm::dvec3{0, 1, -t});
    const auto P8 = projectToSphere(ref, center + glm::dvec3{t, 0, -1});
    const auto P9 = projectToSphere(ref, center + glm::dvec3{t, 0, 1});
    const auto P10 = projectToSphere(ref, center + glm::dvec3{-t, 0, -1});
    const auto P11 = projectToSphere(ref, center + glm::dvec3{-t, 0, 1});

    ts.emplace_back(P0, P11, P5);
    ts.emplace_back(P0, P5, P1);
    ts.emplace_back(P0, P1, P7);
    ts.emplace_back(P0, P7, P10);
    ts.emplace_back(P0, P10, P11);
    ts.emplace_back(P1, P5, P9);
    ts.emplace_back(P5, P11, P4);
    ts.emplace_back(P11, P10, P2);
    ts.emplace_back(P10, P7, P6);
    ts.emplace_back(P7, P1, P8);
    ts.emplace_back(P3, P9, P4);
    ts.emplace_back(P3, P4, P2);
    ts.emplace_back(P3, P2, P6);
    ts.emplace_back(P3, P6, P8);
    ts.emplace_back(P3, P8, P9);
    ts.emplace_back(P4, P9, P5);
    ts.emplace_back(P2, P4, P11);
    ts.emplace_back(P6, P2, P10);
    ts.emplace_back(P8, P6, P7);
    ts.emplace_back(P9, P8, P1);

    // split triangles
    for (auto d = 0; d < subDivisions; d++) {
        ts2.clear();
        for (const auto& t : ts) {
            const auto X = findCenter(ref, t.A, t.B);
            const auto Y = findCenter(ref, t.B, t.C);
            const auto Z = findCenter(ref, t.C, t.A);

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
    std::vector<Triangle> faces;
    faces.reserve(ts.size());
    faces.insert(std::end(faces), std::begin(ts), std::end(ts));
#if 0
    // to obj file
    int vertex = 1;
    for (Triangle face : faces) {
        std::cout << "v " << face.A.x << " " << face.A.y << " " << face.A.z << "\n";
        std::cout << "v " << face.B.x << " " << face.B.y << " " << face.B.z << "\n";
        std::cout << "v " << face.C.x << " " << face.C.y << " " << face.C.z << "\n";

        std::cout << "f " << vertex << " " << vertex + 1 << " " << vertex + 2 << std::endl;
        vertex += 3;
    }
#endif

    return ExplicitEntity(std::move(faces));
}
