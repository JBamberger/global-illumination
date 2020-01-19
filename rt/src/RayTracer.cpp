#include "RayTracer.h"
#include "Material.h"
#include "entities.h"
#include <chrono>
#include <iostream>
#include <limits>

RayTracer::RayTracer(const Camera& camera, std::shared_ptr<const Octree> scene)
    : camera_(camera), scene_(std::move(scene)), image_(std::make_shared<Image>(0, 0)),
      rng_(std::chrono::system_clock::now().time_since_epoch().count()), dist01_(0.0, 1.0),
      dist11_(-1.0, 1.0)
{
}

void RayTracer::setScene(std::shared_ptr<const Octree> scene) { scene_ = std::move(scene); }

void RayTracer::run(int w, int h)
{
    constexpr auto samples = 32;

    image_ = std::make_shared<Image>(w, h);
    camera_.setWindowSize(w, h);
    // The structure of the for loop should remain for incremental rendering.
    for (auto s = 1; s <= samples; ++s) {
        std::cout << "Sample " << s << std::endl;
#pragma omp parallel for schedule(dynamic, 1)
        for (auto y = 0; y < h; ++y) {
            for (auto x = 0; x < w; ++x) {
                if (running_) {
                    const auto ray = camera_.getRay(x, y);
                    const auto color = computePixel(ray);
#pragma omp critical
                    {

                        const auto pc = image_->getPixel(x, y);
                        const auto pix = pc * (static_cast<double>(s - 1) / s) + color * (1.0 / s);
                        image_->setPixel(x, y, pix);
                    }
                }
            }
        }
    }
}

glm::dvec3 RayTracer::computePixel(const int x, const int y)
{
    constexpr auto max_bounces = 10;
    auto ray = camera_.getRay(x, y);

    auto color = glm::dvec3(0, 0, 0);
    auto throughput = glm::dvec3(1, 1, 1);

    for (auto i = 0; i < max_bounces; i++) {
        Hit hit;
        if (!scene_->closestIntersection(ray, hit)) {
            // the ray didn't hit anything -> no contribution.
            break;
        }

        const auto light = hit.mat->emission(hit.uv);
        if (light.x > 0 || light.y > 0 || light.z > 0) {
            color += throughput * light;
        }

        const auto next_dir = hemisphere(hit.normal, ray.dir);
        const auto pdf = glm::one_over_two_pi<double>();

        glm::dvec3 attenuation;
        auto child_ray(ray);
        if (!hit.mat->scatter(ray, hit, attenuation, child_ray)) {
            break;
        }

        const auto mc = attenuation * glm::one_over_pi<double>() * glm::dot(next_dir, hit.normal);
        throughput = throughput * mc / pdf;

        // RR
        const auto p = std::max(throughput.x, std::max(throughput.y, throughput.z));
        if (dist01_(rng_) > p) {
            break;
        }

        // the path survived RR -> remove bias
        throughput *= 1 / p;

        ray = child_ray;
    }

    if (color.x > 1 || color.x < 0 || color.y > 1 || color.y < 0 || color.z > 1 || color.z < 0) {
        std::cout << color.x << " " << color.y << " " << color.z << std::endl;
    }
    return color;
}

glm::dvec3 RayTracer::computePixel(const Ray& ray) const
{
    Hit hit;
    if (!scene_->closestIntersection(ray, hit)) {
        return {0, 0, 0};
    }

    const auto light = hit.mat->emission(hit.uv);

    if (ray.child_level <= 5) {
        glm::dvec3 attenuation;
        auto scatter_ray(ray);
        if (hit.mat->scatter(ray, hit, attenuation, scatter_ray)) {
            return glm::clamp(light + attenuation * computePixel(scatter_ray), 0.0, 1.0);
        }
    }

    return light;
}

glm::dvec3 RayTracer::hemisphere(const glm::dvec3 normal, const glm::dvec3 direction)
{

    const auto r1 = glm::two_pi<double>() * dist01_(rng_);
    const auto r2 = dist01_(rng_);
    const auto sq2 = glm::sqrt(r2);

    const auto x = glm::cos(r1) * sq2;
    const auto y = glm::sin(r1) * sq2;
    const auto z = glm::sqrt(1 - r2);

    // build basis vectors around normal vector
    const auto w = glm::dot(normal, direction) < 0 ? normal : -normal;
    glm::dvec3 c;
    if (std::abs(w.x) < 1 / glm::sqrt(3)) {
        c = glm::dvec3(1, 0, 0);
    } else if (std::abs(w.y) < 1 / glm::sqrt(3)) {
        c = glm::dvec3(0, 1, 0);
    } else {
        c = glm::dvec3(0, 0, 1);
    }
    const auto u = glm::cross(c, w);
    const auto v = glm::cross(w, u);

    // shirley 14. Sampling p294 short form which avoid trig dup. trig function
    return glm::normalize(u * x + v * y + w * z);
}

bool RayTracer::running() const { return running_; }

void RayTracer::stop() { running_ = false; }

void RayTracer::start() { running_ = true; }

std::shared_ptr<Image> RayTracer::get_image() const { return image_; }