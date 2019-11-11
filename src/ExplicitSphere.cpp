#include "entities.h"
#include <iostream>

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

ExplicitSphere::ExplicitSphere(glm::dvec3 center, double radius, int divisions)
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
    for (auto d = 0; d < divisions; d++) {
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
}
