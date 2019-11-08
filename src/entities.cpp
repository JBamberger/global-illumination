
#include "entities.h"

bool ImplicitSphere::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const {
    // O = ray.origin, D = ray.dir, this->radius = R, this->pos = C
    // (P - C)^2 - R^2 = 0 = (O + Dt - C)^2 - R^2
    // O^2 + 2ODt - 2CDt - 2CO + (Dt)^2 + C^2 - R^2 = 0
    // ( D^2 )t^2 + ( 2DO - 2DC )t + ( O^2 - 2DO + C^2 - R^2  ) = 0
    // ( D^2 )t^2 + ( 2D(O - C) )t + ( (O - C)^2 - R^2  ) = 0
    // x1,x2 = (b -+ sqrt( b^2 - 4ac ))/ 2a
    // q = b^2 - 4ac
    // q = (2D(O-C))^2 - 4 * D^2 ((O-C)^2 - R^2)

    glm::dvec3 L = ray.origin - pos;
    double a = glm::dot(ray.dir, ray.dir);
    double b = 2. * glm::dot(ray.dir, L);
    double c = glm::dot(L, L) - (radius * radius);

    // q < 0 -> no solutions
    // q = 0 -> one solution
    // q > 0 -> two solutions
    double q = b * b - 4 * a * c;
    double x1, x2; // possible solutions
    if (q < 0) {
        return false;
    } else if (q == 0) {
        x1 = x2 = -0.5 * b / a;
    } else {
        double root = glm::sqrt(q);
        x1 = 0.5 * (b - root) / a;
        x2 = 0.5 * (b + root) / a;
    }

    // TODO: select correct solution
    intersect = ray.origin + x1 * ray.dir;
    normal = glm::normalize(intersect - pos);
    return true;
}

BoundingBox ImplicitSphere::boundingBox() const { return {pos - radius, pos + radius}; }

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