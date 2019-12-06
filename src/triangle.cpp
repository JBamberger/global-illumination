#include <Triangle.h>

#include <cassert>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

Triangle::Triangle(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c) : A(a), B(b), C(c)
{
    assert(glm::abs(glm::dot(glm::normalize(b - a), glm::normalize(c - a))) != 1);
    updateCaches();
}

bool Triangle::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const
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

BoundingBox Triangle::boundingBox() const
{
    return BoundingBox{glm::min(A, glm::min(B, C)), glm::max(A, glm::max(B, C))};
}

glm::dvec3 Triangle::getColorAtIntersect(glm::dvec3 intersect) const
{
    const auto uv = texMapping(intersect);
    return material->getColor(uv);
}

glm::dvec2 Triangle::texMapping(const glm::dvec3 I) const
{
    // The triangle is given in 3D spaces and forms a 2D plane A + i*AB + j*AC within. The vectors
    // AB and AC form a basis of this subspace. The texture coordinates similarly form a basis of a
    // 2D space. The goal is to perform a change of basis on the intersection point.

    // First the offset must be removed such that a normal change of basis suffices.
    const auto b = I - A;

    // Secondly the coordinate vector of the intersection is computed. The simplest approach is to
    // solve X * c = b for c. Here X is the matrix with AB and AC as columns. Because X is not
    // square we cannot compute the inverse. Additionally the intersection location might not lie on
    // the plane exactly (due to numeric errors). These problems are countered by using the
    // pseudo-inverse X+ instead of X^-1. Because X has full rank (the columns form a basis) X+ can
    // be computed as X+ = (X^T * X)^-1 * X^T. This behaves similarly to using the inverse but
    // solves the linear system of equations in a least-squares sense if no exact solution exists.
    const auto coords = b * to_tex_map;

    // With the coordinate vector the new uv location can be computed directly from the texture
    // basis.
    auto uv = tA + coords.x * tAB + coords.y * tAC;

    // If the intersection point lies within the triangle and the triangle has valid coordinates the
    // result must be between 0 and 1.
    assert(0 <= uv.x && uv.x <= 1);
    assert(0 <= uv.y && uv.y <= 1);
    return uv;
}

void Triangle::setTexCoords(glm::dvec2 Ca, glm::dvec2 Cb, glm::dvec2 Cc)
{
    tA = Ca;
    tAB = Cb - Ca;
    tAC = Cc - Ca;
}

void Triangle::setCoords(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c)
{
    A = a;
    B = b;
    C = c;

    updateCaches();
}

void Triangle::updateCaches()
{
    // recomputes the matrix which computes the coordinates of a point in the triangle plane
    const auto xt = glm::dmat2x3(B - A, C - A);
    to_tex_map = xt * glm::inverse(glm::transpose(xt) * xt);
}
