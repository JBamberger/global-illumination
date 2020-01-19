#pragma once

#include "Entity.h"
#include "Ray.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <random>
#include <utility>

inline glm::dvec3 randomOffset()
{
    static std::default_random_engine rng;
    static std::uniform_real_distribution<double> dist(0, 1);
    glm::dvec3 p;
    do {
        p = glm::dvec3(dist(rng), dist(rng), dist(rng)) * 2.0 - glm::dvec3(1, 1, 1);
    } while (glm::dot(p, p) >= 1.0);
    return p;
}

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