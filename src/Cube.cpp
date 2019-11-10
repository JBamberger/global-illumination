#include "entities.h"

Cube::Cube(glm::dvec3 O, glm::dvec3 A) {
    glm::dvec3 OA = A - O;
    // compute the remaining corner points
    glm::dvec3 B = O + glm::dvec3{OA.x, OA.y, -OA.z};
    glm::dvec3 C = O + glm::dvec3{OA.x, -OA.y, -OA.z};
    glm::dvec3 D = O + glm::dvec3{OA.x, -OA.y, OA.z};

    glm::dvec3 E = O + glm::dvec3{-OA.x, OA.y, OA.z};
    glm::dvec3 F = O + glm::dvec3{-OA.x, OA.y, -OA.z};
    glm::dvec3 G = O + glm::dvec3{-OA.x, -OA.y, -OA.z};
    glm::dvec3 H = O + glm::dvec3{-OA.x, -OA.y, OA.z};

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
}
