#include "Material.h"
#include "RandomUtils.h"
#include <utility>

///************************************************************************************************
/// Lambertian material
///************************************************************************************************

LambertianMaterial::LambertianMaterial(const glm::dvec3& color)
    : tex_(std::make_shared<ConstantTexture>(color))
{
}
LambertianMaterial::LambertianMaterial(std::shared_ptr<Texture> tex) : tex_(std::move(tex)) {}

bool LambertianMaterial::scatter(const Ray& in,
                                 const Hit& ir,
                                 glm::dvec3& attenuation,
                                 Ray& scatter_ray) const
{

    const auto target = ir.pos + ir.normal + randomOffset();
    scatter_ray = in.getChildRay(ir.pos, target - ir.pos);
    attenuation = tex_->value(ir.uv);
    return true;
}

///************************************************************************************************
/// DiffuseLight
///************************************************************************************************

DiffuseLight::DiffuseLight(const glm::dvec3& color) : LambertianMaterial(color) {}

DiffuseLight::DiffuseLight(std::shared_ptr<Texture> tex) : LambertianMaterial(std::move(tex)) {}

glm::dvec3 DiffuseLight::emission(const glm::dvec2& uv) const { return tex_->value(uv); }

///************************************************************************************************
/// Metal-like material
///************************************************************************************************

MetalLikeMaterial::MetalLikeMaterial(const glm::dvec3& attenuation, const double spec_size)
    : attenuation_(attenuation), spec_size_(spec_size)
{
    assert(-1.0 <= spec_size_ && spec_size_ <= 1.0);
}

bool MetalLikeMaterial::scatter(const Ray& in,
                                const Hit& ir,
                                glm::dvec3& attenuation,
                                Ray& scatter_ray) const
{
    // compute the reflection and then randomly offset the reflection based on the size of the
    // specular highlight.
    const auto direction = glm::reflect(in.dir, ir.normal) + spec_size_ * randomOffset();

    scatter_ray = in.getChildRay(ir.pos, direction);
    attenuation = attenuation_;

    // if the reflection does not point in the same direction as the normal it is not used.
    return glm::dot(scatter_ray.dir, ir.normal) > 0;
}

///************************************************************************************************
/// Dielectric material
///************************************************************************************************

Dielectric::Dielectric(const double refractive_index) : refractive_index_(refractive_index) {}

bool Dielectric::scatter(const Ray& in,
                         const Hit& ir,
                         glm::dvec3& attenuation,
                         Ray& scatter_ray) const
{
    attenuation = glm::dvec3(1, 1, 1); // perfect forwarding
    const auto reflected = glm::reflect(in.dir, ir.normal);
    glm::dvec3 n;
    double eta;
    double cosine = glm::dot(in.dir, ir.normal) / glm::length(in.dir);
    if (glm::dot(in.dir, ir.normal) > 0) {
        n = -ir.normal;
        eta = refractive_index_;
        cosine = refractive_index_ * cosine;
    } else {
        n = ir.normal;
        eta = 1 / refractive_index_;
        cosine = -cosine;
    }

    const auto dt = glm::dot(in.dir, n);
    const auto k = 1.0 - eta * eta * (1 - dt * dt);
    if (k >= 0) { // is refraction possible?
        const auto refracted = eta * (in.dir - n * dt) - n * glm::sqrt(k);

        // Schlick's approximation for Fresnel effect / angle-dependent reflection/refraction
        auto r0 = (1 - eta) / (1 + eta);
        r0 = r0 * r0;
        const auto ref_prb = r0 + (1 - r0) * glm::pow(1 - cosine, 5);

        // randomly decide if the ray should reflect or refract based on computed probability
        scatter_ray = in.getChildRay(ir.pos, rng() < ref_prb ? reflected : refracted);
    } else {
        scatter_ray = in.getChildRay(ir.pos, reflected);
    }

    return true;
}
