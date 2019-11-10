#include "entities.h"

Triangle::Triangle() : Entity() {}

Triangle::Triangle(const Material& material) : Entity(material) {}

Triangle::Triangle(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c) : A(a), B(b), C(c) {}

bool Triangle::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const {
    glm::dvec3 N = glm::cross(B - A, C - A);

    const double DN = glm::dot(N, ray.dir);

    // discard rays that are nearly parallel to the triangle
    if (glm::abs(DN) <= 1e-11) {
        return false;
    }

    const double p = glm::dot(A - ray.origin, N) / DN;
    const glm::dvec3 I = ray.origin + p * ray.dir;

    // Ray intersects plane at I, perform half-space tests

    if (glm::dot(N, glm::cross(B - A, I - A)) <= 0)
        return false;
    if (glm::dot(N, glm::cross(C - B, I - B)) <= 0)
        return false;
    if (glm::dot(N, glm::cross(A - C, I - C)) <= 0)
        return false;

    intersect = I;
    normal = N;
    return true;
}

BoundingBox Triangle::boundingBox() const {
    return BoundingBox{glm::min(A, glm::min(B, C)), glm::max(A, glm::max(B, C))};
}