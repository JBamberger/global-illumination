#include "triangle.h"
#include "glm/gtc/epsilon.hpp"

triangle::triangle() : entity() {}

triangle::triangle(const Material& material) : entity(material) {}

triangle::triangle(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c) : A(a), B(b), C(c) {}

bool triangle::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const
{
    const auto N = this->normal();
    const auto DN = glm::dot(N, ray.dir);

    // discard rays that are nearly parallel to the triangle
    if (glm::epsilonEqual(0., DN, 1e-11)) {
        return false;
    }

    const auto c = glm::dot(A - ray.origin, N) / DN;

    // test if the triangle is behind the ray origin
    if (c <= 0)
        return false;

    // compute the intersection point
    const auto I = ray.origin + c * ray.dir;

    // Ray intersects plane at I, perform half-space tests
    if (glm::dot(N, glm::cross(B - A, I - A)) < 0)
        return false;
    if (glm::dot(N, glm::cross(C - B, I - B)) < 0)
        return false;
    if (glm::dot(N, glm::cross(A - C, I - C)) < 0)
        return false;

    intersect = I;
    normal = N;
    return true;
}

BoundingBox triangle::boundingBox() const
{
    return BoundingBox{glm::min(A, glm::min(B, C)), glm::max(A, glm::max(B, C))};
}