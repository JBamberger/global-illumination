#include "entities.h"

// projects a point to the given sphere
inline glm::dvec3 projectToSphere(const ImplicitSphere& s, glm::dvec3 P) {
    const auto r = Ray{s.pos, P - s.pos};
    glm::dvec3 i, n;
    const auto success = s.intersect(r, i, n);

    assert(success);
    return i;
}

/// computes the middle point between two points and projects it to the sphere
inline glm::dvec3 findCenter(const ImplicitSphere& s, glm::dvec3 p1, glm::dvec3 p2) {
    return projectToSphere(s, p2 - p1);
}

ExplicitSphere::ExplicitSphere(glm::dvec3 center, double radius, int divisions) {
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
    const auto P0 = projectToSphere(ref, center + glm::dvec3{1, 1, 1});
    const auto P1 = projectToSphere(ref, center + glm::dvec3{-1, -1, 1});
    const auto P2 = projectToSphere(ref, center + glm::dvec3{-1, 1, -1});
    const auto P3 = projectToSphere(ref, center + glm::dvec3{1, -1, -1});
    ts.emplace_back(P0, P1, P2);
    ts.emplace_back(P1, P3, P2);
    ts.emplace_back(P0, P3, P2);
    ts.emplace_back(P1, P3, P0);

    // split triangles
    for (auto d = 0; d < divisions; d++) {
        for (const auto& t : ts) {
            const auto X = findCenter(ref, t.A, t.B);
            const auto Y = findCenter(ref, t.B, t.C);
            const auto Z = findCenter(ref, t.C, t.A);

            ts2.emplace_back(t.A, X, Z);
            ts2.emplace_back(X, t.B, Y);
            ts2.emplace_back(Y, t.C, Z);
            ts2.emplace_back(X, Y, Z);
        }
        std::swap(ts, ts2);
    }
    faces.reserve(ts.size());
    faces.insert(std::end(faces), std::begin(ts), std::end(ts));
}
