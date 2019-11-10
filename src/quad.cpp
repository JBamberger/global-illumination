#include "entities.h"

Quad::Quad(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C, glm::dvec3 D) {
    faces.emplace_back(A, B, C);
    faces.emplace_back(C, D, A);

    // TODO: assert that points lie in plane
}
