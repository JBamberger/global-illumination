#include <ImplicitSphere.h>

#include <glm/gtc/constants.hpp>

ImplicitSphere::ImplicitSphere(const glm::dvec3 center, const double radius)
    : center(center), radius(radius)
{
}

const Entity* ImplicitSphere::intersect(const Ray& ray,
                                        glm::dvec3& intersect,
                                        glm::dvec3& normal) const
{
    // O = ray.origin
    // D = ray.dir
    // R = this->radius
    // C = this->center

    // Sphere equation: (P - C)^2 - R^2 = 0
    // Ray equation: P = O + Dt
    //
    // Insert ray into sphere equation:
    // 0 = (O + Dt - C)^2 - R^2
    //
    // Expand and group by t:
    // ( D^2 )t^2 + ( 2D(O - C) )t + ( (O - C)^2 - R^2  ) = 0
    //
    // a := D^2
    // b := 2D(O - C)
    // c := (O - C)^2 - R^2
    //
    // Solve quadratic equation at^2 + bt + c = 0
    // using x1,x2 = (b -+ sqrt( b^2 - 4ac )) / 2a
    //
    // q := b^2 - 4ac
    //
    // q < 0 -> no solutions
    // q = 0 -> one solution
    // q > 0 -> two solutions

    const auto OC = ray.origin - center;
    const auto a = glm::dot(ray.dir, ray.dir);
    const auto b = 2. * glm::dot(ray.dir, OC);
    const auto c = glm::dot(OC, OC) - (radius * radius);

    const auto q = b * b - 4 * a * c;

    // There exists no solution. The ray missed the sphere.
    if (q < 0) {
        return nullptr;
    }

    double solution; // possible solutions
    if (q == 0) {
        // There exists only one solution. The ray hit the sphere tangential.
        solution = -0.5 * b / a;
    } else {
        // There are two possible solutions. The ray hit the sphere non-tangential.
        const auto root = glm::sqrt(q);
        const auto x1 = -0.5 * (b - root) / a;
        const auto x2 = -0.5 * (b + root) / a;

        // The solution must be geq 0 because the intersection should be in front of the ray origin.
        // Additionally we want the intersection location closer to the ray origin, hence the
        // selected value must be the smaller one.
        solution = glm::min(x1, x2);
        if (solution <= 0) {
            solution = glm::max(x1, x2);
            if (solution <= 0) {
                return false; // both intersections are behind the ray origin
            }
        }
    }

    intersect = ray.origin + solution * ray.dir;
    normal = glm::normalize(intersect - center);
    return this;
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
