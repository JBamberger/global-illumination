#pragma once

#include "Entity.h"
#include "RandomUtils.h"
#include "Ray.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <utility>

class Material {
  public:
    virtual ~Material() = default;

    /**
     * \brief Computes a new scattered ray given an input and hit.
     * \param in The incoming ray that hit the material
     * \param ir intersection record describing the hit properties
     * \param attenuation outputs the damping of the light transport for the scattered ray
     * \param scatter_ray the new scattered ray
     * \return true if there is a scattered ray
     */
    virtual bool scatter(const Ray& in,
                         const Hit& ir,
                         glm::dvec3& attenuation,
                         Ray& scatter_ray) const = 0;

    /**
     * \brief Returns the emission of the given material
     * \param uv uv coordinates of of the emission position
     * \return a color vector describing the emission per channel
     */
    virtual glm::dvec3 emission(const glm::dvec2& uv) const { return glm::dvec3(0, 0, 0); }
};

/**
 * \brief Material that has Lambertian scattering properties.
 *
 * The material scatters the rays into the hemisphere uniformly.
 */
class LambertianMaterial : public Material {
  protected:
    std::shared_ptr<Texture> tex_;

  public:
    explicit LambertianMaterial(const glm::dvec3& color)
        : tex_(std::make_shared<ConstantTexture>(color))
    {
    }
    explicit LambertianMaterial(std::shared_ptr<Texture> tex) : tex_(std::move(tex)) {}

    bool scatter(const Ray& in,
                 const Hit& ir,
                 glm::dvec3& attenuation,
                 Ray& scatter_ray) const override
    {

        const auto target = ir.pos + ir.normal + randomOffset();
        scatter_ray = in.getChildRay(ir.pos, target - ir.pos);
        attenuation = tex_->value(ir.uv);
        return true;
    }
};

class DiffuseLight final : public LambertianMaterial {
  public:
    explicit DiffuseLight(const glm::dvec3& color) : LambertianMaterial(color) {}
    explicit DiffuseLight(std::shared_ptr<Texture> tex) : LambertianMaterial(std::move(tex)) {}
    glm::dvec3 emission(const glm::dvec2& uv) const override { return tex_->value(uv); }
};

class MetalLikeMaterial final : public Material {
    glm::dvec3 attenuation_;
    double spec_size_;

  public:
    MetalLikeMaterial(const glm::dvec3& attenuation, const double spec_size)
        : attenuation_(attenuation), spec_size_(spec_size)
    {
        assert(-1.0 <= spec_size_ && spec_size_ <= 1.0);
    }

    bool scatter(const Ray& in,
                 const Hit& ir,
                 glm::dvec3& attenuation,
                 Ray& scatter_ray) const override
    {
        // compute the reflection and then randomly offset the reflection based on the size of the
        // specular highlight.
        const auto direction = glm::reflect(in.dir, ir.normal) + spec_size_ * randomOffset();

        scatter_ray = in.getChildRay(ir.pos, direction);
        attenuation = attenuation_;

        // if the reflection does not point in the same direction as the normal it is not used.
        return glm::dot(scatter_ray.dir, ir.normal) > 0;
    }
};

class Dielectric final : public Material {
    double refractive_index_;

  public:
    explicit Dielectric(const double refractive_index) : refractive_index_(refractive_index) {}
    bool scatter(const Ray& in,
                 const Hit& ir,
                 glm::dvec3& attenuation,
                 Ray& scatter_ray) const override
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
};