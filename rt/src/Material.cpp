/**
 *    Copyright 2020 Jannik Bamberger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

    // If the reflection does not point in the same direction as the normal it is not used.
    // In this case this are rays that are tangential to the surface.
    return glm::dot(scatter_ray.dir, ir.normal) > 0;
}

///************************************************************************************************
/// Dielectric material
///************************************************************************************************

double Dielectric::reflectance_schlick(double n1, double n2, double cosI)
{
    auto r0 = (n1 - n2) / (n1 + n2);
    r0 = r0 * r0;
    const auto a = 1.0 - cosI;
    return r0 + (1.0 - r0) * a * a * a * a * a;
}

double Dielectric::reflectance_fresnel(double n1, double n2, double cosI, double cosT)
{
    const auto r_orth = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
    const auto r_par = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
    return (r_orth * r_orth + r_par * r_par) / 2.0;
}

Dielectric::Dielectric(const double refractive_index) : refractive_index_(refractive_index) {}

bool Dielectric::scatter(const Ray& in,
                         const Hit& ir,
                         glm::dvec3& attenuation,
                         Ray& scatter_ray) const
{

    glm::dvec3 n;
    double n1, n2;
    // decide if the ray is entering or leaving the object.
    if (glm::dot(in.dir, ir.normal) > 0) {
        // The ray exits the ray -> flip normal to point towards the ray
        n = -ir.normal;
        n1 = refractive_index_;
        n2 = 1.0;
    } else {
        // The ray exits the object -> the normal is oriented correctly
        n = ir.normal;
        n1 = 1.0;
        n2 = refractive_index_;
    }

    const auto eta = n1 / n2;
    const auto dt = glm::dot(in.dir, n);

    // this direction decides if the ray is reflected or refracted.
    auto out_direction = glm::reflect(in.dir, ir.normal);

    // the refraction is conditioned on this value which is located below the square root later on.
    // Hence the value must be geq 0 or no refraction is possible.
    const auto k = 1.0 - eta * eta * (1 - dt * dt);

    // If k < 0 then we cannot compute a refraction ray. This can be seen easily when looking at
    // Snell's law sin(theta1) = n1 / n2 * sin(theta2). If sin(theta1) is larger than n1/n2 then the
    // other sine value would need to be larger than one. Since this is impossible we cannot compute
    // a refraction in such cases. This is known as total internal reflection.
    if (k >= 0) { // otherwise total internal reflection -> ref_prb = 1;
        const auto cosI = -glm::dot(in.dir, n);
        const auto cosT = glm::sqrt(k);

        const auto ref_prb = reflectance_schlick(n1, n2, cosI);
        // const auto ref_prb = reflectance_fresnel(n1, n2, cosI, cosT);

        // randomly decide if the ray should reflect or refract based on computed probability
        if (rng() >= ref_prb) {
            // the ray is refracted, compute the refraction direction
            out_direction = eta * (in.dir - n * dt) - n * cosT;
        }
    }

    // All light is either reflected or refracted, no attenuation takes place.
    attenuation = glm::dvec3(1, 1, 1);
    scatter_ray = in.getChildRay(ir.pos, out_direction);

    return true; // The ray is never absorbed in a dielectric material.
}
