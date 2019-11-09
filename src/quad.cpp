#include "entities.h"

Quad::Quad(glm::dvec3 A, glm::dvec3 B, glm::dvec3 C, glm::dvec3 D) {
    T1.A = A;
    T1.B = B;
    T1.C = C;
    T2.A = C;
    T2.B = D;
    T2.C = A;
    // TODO: assert that points lie in plane
}

bool Quad::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const {
    if (T1.intersect(ray, intersect, normal))
        return true;
    if (T2.intersect(ray, intersect, normal))
        return true;
    return false;
}

BoundingBox Quad::boundingBox() const {
    return BoundingBox::unite(T1.boundingBox(), T2.boundingBox());
}