#include "Entity.h"
#include "Material.h"

Hit::Hit() = default;

///************************************************************************************************
/// Entity
///************************************************************************************************

Entity::Entity()
    : Entity(std::make_shared<LambertianMaterial>(
          std::make_shared<ConstantTexture>(glm::dvec3{1, 0, 0})))
{
}

Entity::Entity(std::shared_ptr<Material> material) : material_(std::move(material)) {}

void Entity::setMaterial(std::shared_ptr<Material> material)
{
    this->material_ = std::move(material);
}

///************************************************************************************************
/// Triangle
///************************************************************************************************

Triangle::Triangle(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c) : A(a), B(b), C(c)
{
    assert(glm::abs(glm::dot(glm::normalize(b - a), glm::normalize(c - a))) != 1);
    updateCaches();
}

bool Triangle::intersect(const Ray& ray, Hit& hit) const
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
    // The cross product points in the same direction as the normal if the point is inside of
    // the half-space
    if (glm::dot(N, glm::cross(B - A, I - A)) < 0)
        return false;
    if (glm::dot(N, glm::cross(C - B, I - B)) < 0)
        return false;
    if (glm::dot(N, glm::cross(A - C, I - C)) < 0)
        return false;

    hit.normal = N;
    hit.pos = I;
    hit.uv = texMapping(I);
    hit.mat = material_;

    return true;
}

BoundingBox Triangle::boundingBox() const
{
    return BoundingBox{glm::min(A, glm::min(B, C)), glm::max(A, glm::max(B, C))};
}

glm::dvec3 Triangle::normal() const { return glm::normalize(glm::cross(B - A, C - A)); }

glm::dvec2 Triangle::texMapping(const glm::dvec3& intersect) const
{
    // The triangle is given in 3D spaces and forms a 2D plane A + i*AB + j*AC within. The
    // vectors AB and AC form a basis of this subspace. The texture coordinates similarly form a
    // basis of a 2D space. The goal is to perform a change of basis on the intersection point.

    // First the offset must be removed such that a normal change of basis suffices.
    const auto b = intersect - A;

    // Secondly the coordinate vector of the intersection is computed. The simplest approach is
    // to solve X * c = b for c. Here X is the matrix with AB and AC as columns. Because X is
    // not square we cannot compute the inverse. Additionally the intersection location might
    // not lie on the plane exactly (due to numeric errors). These problems are countered by
    // using the pseudo-inverse X+ instead of X^-1. Because X has full rank (the columns form a
    // basis) X+ can be computed as X+ = (X^T * X)^-1 * X^T. This behaves similarly to using the
    // inverse but solves the linear system of equations in a least-squares sense if no exact
    // solution exists.
    const auto coords = b * to_tex_map;

    // With the coordinate vector the new uv location can be computed directly from the texture
    // basis.
    auto uv = tA + coords.x * tAB + coords.y * tAC;

    // If the intersection point lies within the triangle and the triangle has valid coordinates
    // the result must be between 0 and 1.
    assert(-0.00001 <= uv.x && uv.x <= 1.00001);
    assert(-0.00001 <= uv.y && uv.y <= 1.00001);
    return glm::clamp(uv, 0.0, 1.0);
}

void Triangle::setTexCoords(const glm::dvec2 ca, const glm::dvec2 cb, const glm::dvec2 cc)
{
    tA = ca;
    tAB = cb - ca;
    tAC = cc - ca;
}

void Triangle::setCoords(const glm::dvec3 a, const glm::dvec3 b, const glm::dvec3 c)
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

///************************************************************************************************
/// Sphere
///************************************************************************************************

Sphere::Sphere() : Sphere({0, 0, 0}, 1) {}

Sphere::Sphere(const glm::dvec3 center, const double radius) : center(center), radius(radius) {}

bool Sphere::intersect(const Ray& ray, Hit& hit) const
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
        return false;
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

        // The solution must be geq 0 because the intersection should be in front of the ray
        // origin. Additionally we want the intersection location closer to the ray origin,
        // hence the selected value must be the smaller one.
        solution = glm::min(x1, x2);
        if (solution <= 0) {
            solution = glm::max(x1, x2);
            if (solution <= 0) {
                return false; // both intersections are behind the ray origin
            }
        }
    }

    hit.pos = ray.origin + solution * ray.dir;
    hit.normal = glm::normalize(hit.pos - center);
    hit.mat = material_;
    hit.uv = texMapping(hit.pos);
    return true;
}

BoundingBox Sphere::boundingBox() const { return {center - radius, center + radius}; }

glm::dvec2 Sphere::texMapping(const glm::dvec3& intersect) const
{
    const auto n = glm::normalize(intersect - center);

#ifdef CYLINDRICAL_PROJECTION
    const auto u = atan2(n.x, n.z) / (2 * glm::pi<double>()) + 0.5;
    const auto v = n.y * 0.5 + 0.5;
#elif AXIAL_PROJECTION
    const auto u = n.z * 0.5 + 0.5;
    const auto v = n.y * 0.5 + 0.5;
#elif SPHERICAL_PROJECTION
    const auto u = 0.5 + atan2(n.x, n.z) / (2 * glm::pi<double>());
    const auto v = 0.5 + atan2(n.x, n.y) / (2 * glm::pi<double>());
#else
    // from https://en.wikipedia.org/wiki/UV_mapping
    const auto u = 0.5 + atan2(n.x, n.z) * glm::one_over_two_pi<double>();
    const auto v = 0.5 - asin(n.y) * glm::one_over_pi<double>();
#endif
    return {u, v};
}
