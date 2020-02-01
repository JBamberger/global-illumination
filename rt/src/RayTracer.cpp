#include "RayTracer.h"
#include "Material.h"
#include "entities.h"
#include <chrono>
#include <iostream>

RayTracer::RayTracer(const Camera& camera, std::shared_ptr<const Octree> scene)
    : camera_(camera), scene_(std::move(scene)), image_(std::make_shared<Image>(0, 0))
{
}

void RayTracer::setScene(std::shared_ptr<const Octree> scene) { scene_ = std::move(scene); }

void RayTracer::run(const int w, const int h)
{
    constexpr auto samples = 2048;

    std::vector<glm::dvec3> buffer;
    buffer.reserve(w * h);
    for (auto i = 0; i < w * h; i++) {
        buffer.emplace_back(0, 0, 0);
    }

    image_ = std::make_shared<Image>(w, h);
    camera_.setWindowSize(w, h);
    // The structure of the for loop should remain for incremental rendering.
    for (auto s = 1; s <= samples; ++s) {
        if (!running_) {
            return;
        }
        std::cout << "Sample " << s << std::endl;
#pragma omp parallel for schedule(dynamic, 1)
        for (auto y = 0; y < h; ++y) {
            for (auto x = 0; x < w; ++x) {
                if (running_) {
                    const auto color = computePixel(x, y);
#pragma omp critical
                    {
                        const auto pos = x * h + y;
                        buffer[pos] += color;
                        const auto pix = buffer[pos] / static_cast<double>(s);
                        image_->setPixel(x, y, glm::clamp(pix, 0.0, 1.0));
                    }
                }
            }
        }
    }
}

glm::dvec3 RayTracer::computePixel(const int x, const int y) const
{
    constexpr auto max_bounces = 5;

    // the currently active ray
    auto ray = camera_.getRay(x, y);
    // the total amount of light carried over this path
    auto light = glm::dvec3(0, 0, 0);
    // value gives the amount of light that is carried per color channel over the path
    auto throughput = glm::dvec3(1, 1, 1);

    for (auto i = 0; i < max_bounces; i++) {
        Hit hit;
        if (!scene_->intersect(ray, hit)) {
            break; // the ray didn't hit anything -> no contribution.
        }

        // add light reduced by combined attenuation
        light += throughput * hit.mat->emission(hit.uv);

        glm::dvec3 bounce_attenuation;
        auto scatter_ray(ray);
        if (!hit.mat->scatter(ray, hit, bounce_attenuation, scatter_ray)) {
            break; // the ray did not scatter -> no further contribution
        }
        ray = scatter_ray;
        throughput *= bounce_attenuation;
    }

    return light;
}

glm::dvec3 RayTracer::computePixel(const Ray& ray) const
{
    Hit hit;
    if (!scene_->intersect(ray, hit)) {
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

bool RayTracer::running() const { return running_; }

void RayTracer::stop() { running_ = false; }

void RayTracer::start() { running_ = true; }

std::shared_ptr<Image> RayTracer::getImage() const { return image_; }
