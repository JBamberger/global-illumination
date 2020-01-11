#include "NoiseTexture.h"
#include <RayTracer.h>
#include <iostream>
#include <random>

RayTracer::RayTracer(const Camera& camera, glm::dvec3 light)
    : camera_(camera), light_(light), image_(std::make_shared<Image>(0, 0))
{
}

void RayTracer::set_scene(const Octree* scene) { scene_ = scene; }

void RayTracer::run(int w, int h)
{
    image_ = std::make_shared<Image>(w, h);
    camera_.set_window_size(w, h);
// The structure of the for loop should remain for incremental rendering.
#pragma omp parallel for schedule(dynamic, 4)
    for (auto y = 0; y < h; ++y) {
        for (auto x = 0; x < w; ++x) {
            if (running_) {
                const auto ray = camera_.get_ray(x, y);
                const auto pix = compute_pixel(ray);

#pragma omp critical
                image_->setPixel(x, y, pix);
            }
        }
    }
}

glm::dvec3 RayTracer::compute_pixel(const Ray& ray) const
{
    glm::dvec3 color{0, 0, 0}; // background color

    if (ray.child_level >= 5) {
        return glm::dvec3{0, 1, 0};
    }

    glm::dvec3 intersect, normal; // values at minimum
    auto min_ent = scene_->closestIntersection(ray, intersect, normal);

    if (min_ent == nullptr)
        return color;

    // this is the entity material at the intersection location
    const auto mat = min_ent->material;

    // this is the base color value of the entity at the intersection location
    const auto color_at_intersect = min_ent->getColorAtIntersect(intersect);

    // compute light and normal vector at the intersection point
    const auto l = glm::normalize(light_ - intersect);

    // check if the light is obstructed by some an entity
    const auto blocked = scene_->isBlocked(ray.getChildRay(intersect, l), light_);

    // ambient: L_a = k_a * I_a
    color = color + color_at_intersect * mat->ambient;

    if (!blocked) {
        // direct light is only considered if the light is not blocked

        { // diffuse:  L_d = k_d * I * max(0.0, dot(n, l))
            const auto diffuse = glm::dot(normal, l);
            if (diffuse > 0) {
                color += color_at_intersect * mat->diffuse * diffuse;
            }
        }

        { // specular (Blinn-Phong): L_s = k_s * I * max(0.0, dot(n,normalize(v + l)))^p
            const auto v = -ray.dir;                     // eye direction
            const auto bisector = glm::normalize(v + l); // center between view and light
            const auto base = glm::dot(normal, bisector);
            if (base > 0) {
                color += mat->specular * glm::pow(base, mat->specular_exponent);
            }
        }
    }

    { // refractive: (trace another ray in refraction direction)
        // It is not clear what should happen in the intersection of two objects. Which
        // material / refractive index is used?

        if (mat->refractive > 0) {
            auto n = normal;
            auto m2 = mat->refractive_index;

            // check it normal and ray point in the same direction, i.e. if the ray is inside or
            // outside of the object
            if (glm::dot(ray.dir, normal) > 0) {
                n = -n;
                m2 = 1.0; // exiting, i.e. reset to air
            }

            // ratio of refraction indices
            const auto eta = ray.refractive_index / m2;

            const auto refract_dir = glm::refract(ray.dir, n, eta);
            auto refract_ray = ray.getChildRay(intersect, refract_dir);

            // the child ray is in a new material -> fix the refraction index
            refract_ray.refractive_index = m2;

            const auto refraction = compute_pixel(refract_ray);

            color += mat->refractive * refraction;
        }
    }

    { // rough and reflective surfaces
        if (mat->reflective > 0) {
            const auto reflect_dir = glm::normalize(glm::reflect(ray.dir, normal));
            glm::dvec3 avg_color;
            if (mat->rough_radius > 0) {
                // rough surfaces: L = k * avg(trace_n(Ray(P, r + rand()))
                //
                // find a vector that is not linearly dependent of reflect_dir
                const auto d = glm::abs(glm::dot(reflect_dir, {1, 0, 0})) != 1
                                   ? glm::dvec3{1, 0, 0}
                                   : glm::dvec3{0, 1, 0};

                const auto a = glm::normalize(glm::cross(reflect_dir, d));
                const auto b = glm::normalize(glm::cross(reflect_dir, a));

                static std::default_random_engine rng;
                static std::uniform_real_distribution<double> angle_distribution(
                    0.0, 2 * glm::pi<double>());

                std::uniform_real_distribution<double> radius_distribution(0.0, mat->rough_radius);

                avg_color = glm::dvec3(0, 0, 0);
                for (size_t i = 0; i < mat->reflect_rays; i++) {
                    // Actually a glm::sqrt() would be necessary to obtain a real uniform
                    // distribution. In this case the center-heavy distribution does no harm and is
                    // computationally cheaper.
                    const auto radius = radius_distribution(rng);
                    const auto theta = angle_distribution(rng);
                    const auto deflection = radius * (sin(theta) * a + cos(theta) * b);
                    const auto rough_ray = ray.getChildRay(intersect, reflect_dir + deflection);
                    avg_color += compute_pixel(rough_ray);
                }
                avg_color = avg_color / static_cast<double>(mat->reflect_rays);
            } else {
                // reflective: L_m = k_m * trace(Ray(P, r))
                avg_color = compute_pixel(ray.getChildRay(intersect, reflect_dir));
            }

            color += mat->reflective * avg_color;
        }
    }

    // the color value must be clamped because otherwise high illumination will produce
    // values above 1. This results in errors from Qt.
    color = glm::clamp(color, 0., 1.);

    return color;
}

bool RayTracer::running() const { return running_; }

void RayTracer::stop() { running_ = false; }

void RayTracer::start() { running_ = true; }

std::shared_ptr<Image> RayTracer::get_image() const { return image_; }