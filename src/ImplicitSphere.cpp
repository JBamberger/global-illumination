#include <ImplicitSphere.h>

#include "glm/gtc/constants.hpp"

ImplicitSphere::ImplicitSphere(const glm::dvec3 center, const double radius)
    : center(center), radius(radius)
{
}

bool ImplicitSphere::intersect(const Ray& ray, glm::dvec3& intersect, glm::dvec3& normal) const
{
    // O = ray.origin
    // D = ray.dir
    // R = this->radius
    // C = this->center
    //
    // (P - C)^2 - R^2 = 0 = (O + Dt - C)^2 - R^2
    // O^2 + 2ODt - 2CDt - 2CO + (Dt)^2 + C^2 - R^2 = 0
    // ( D^2 )t^2 + ( 2DO - 2DC )t + ( O^2 - 2DO + C^2 - R^2  ) = 0
    // ( D^2 )t^2 + ( 2D(O - C) )t + ( (O - C)^2 - R^2  ) = 0
    //
    // x1,x2 = (b -+ sqrt( b^2 - 4ac ))/ 2a
    // q = b^2 - 4ac
    // q = (2D(O-C))^2 - 4 * D^2 ((O-C)^2 - R^2)

    const auto L = ray.origin - center;
    const auto a = glm::dot(ray.dir, ray.dir);
    const auto b = 2. * glm::dot(ray.dir, L);
    const auto c = glm::dot(L, L) - (radius * radius);

    // q < 0 -> no solutions
    // q = 0 -> one solution
    // q > 0 -> two solutions
    const auto q = b * b - 4 * a * c;
    double solution; // possible solutions
    if (q < 0) {
        return false;
    }
    if (q == 0) {
        solution = -0.5 * b / a;
    } else {
        const auto root = glm::sqrt(q);
        const auto x1 = -0.5 * (b - root) / a;
        const auto x2 = -0.5 * (b + root) / a;

        if (x1 <= 0) {
            if (x2 <= 0) {
                return false;
            }
            solution = x2;
        } else {
            solution = x1;
            if (x2 > 0 && x2 < x1) {
                solution = x2;
            }
        }
    }

    intersect = ray.origin + solution * ray.dir;
    normal = glm::normalize(intersect - center);
    return true;
}

BoundingBox ImplicitSphere::boundingBox() const { return {center - radius, center + radius}; }

glm::dvec3 ImplicitSphere::getColorAtIntersect(const glm::dvec3 intersect) const
{
    const auto n = glm::normalize(intersect - center);

#ifdef CYLINDRICAL_PROJECTION
    const auto u = atan2(n.x, n.z) / (2 * glm::pi<double>()) + 0.5;
    const auto v = n.y * 0.5 + 0.5;
#elif AXIAL_PROJECTION
    const auto u = n.z * 0.5 + 0.5;
    const auto v = n.y * 0.5 + 0.5;
#elseif SPHERICAL_PROJECTION
    const auto u = 0.5 + atan2(n.x, n.z) / (2 * glm::pi<double>());
    const auto v = 0.5 + atan2(n.x, n.y) / (2 * glm::pi<double>());
#else
    // from https://en.wikipedia.org/wiki/UV_mapping
    const auto u = 0.5 + atan2(n.x, n.z) / (2 * glm::pi<double>());
    const auto v = 0.5 - asin(n.y) / glm::pi<double>();
#endif

    return material->getColor({u, v});
}
