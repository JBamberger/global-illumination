#include "triangle.h"
#include "glm/gtc/epsilon.hpp"
#include <cassert>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

triangle::triangle(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c) : A(a), B(b), C(c)
{
    assert(glm::abs(glm::dot(glm::normalize(b - a), glm::normalize(c - a))) != 1);
}

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
    // The cross product points in the same direction as the normal if the point is inside of the
    // half-space
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

glm::dvec3 triangle::get_color_at_intersect(glm::dvec3 intersect) const
{
    const auto uv = tex_mapping(intersect);
    return material->get_color(uv);
}

glm::dvec2 triangle::tex_mapping(const glm::dvec3 I) const
{
    // p = A + i*BA + j*CA
    // q = t1 + i*t12t1 + j*t3t1

    // p - A = i*BA - j*CA

    const auto AB = B - A;
    const auto AC = C - A;
    const auto b = I - A;

    const auto X = glm::dmat3x2(AB.x, AC.x, AB.y, AC.y, AB.z, AC.z);

    const glm::dmat2x3 XT = glm::transpose(X);
    const glm::dmat2x2 XC = X * XT;
    const glm::dmat2x2 XI = glm::inverse(XC);
    const glm::dmat2x3 MoorePenrose = XT * XI;

    const glm::dvec2 coords = b * MoorePenrose;
    const auto u = coords.x;
    const auto v = coords.y;

    // const auto N = glm::cross(B - A, C - A);
    // const auto norm = glm::dot(N, N);
    // const auto u = glm::dot(N, glm::cross(C - B, I - B)) / norm;
    // const auto v = glm::dot(N, glm::cross(A - C, I - C)) / norm;

    auto uv = t1 + u * (t2 - t1) + v * (t3 - t1);
    // assert(-0.1 <= uv.x && uv.x <= 1.1);
    // assert(-0.1 <= uv.y && uv.y <= 1.1);
    // uv.x = glm::clamp(uv.x, 0.0, 1.0);
    // uv.y = glm::clamp(1 - uv.y, 0.0, 1.0);
    assert(0 <= uv.x && uv.x <= 1);
    assert(0 <= uv.y && uv.y <= 1);
    return uv;
}
