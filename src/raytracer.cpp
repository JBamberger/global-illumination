#include <raytracer.h>

RayTracer::RayTracer(const Camera& camera, glm::dvec3 light)
    : camera_(camera), light_(light), image_(std::make_shared<Image>(0, 0))
{
}

void RayTracer::set_scene(const octree* scene) { scene_ = scene; }

void RayTracer::run(int w, int h)
{
    image_ = std::make_shared<Image>(w, h);
    camera_.set_window_size(w, h);
// The structure of the for loop should remain for incremental rendering.
// termination criterion: add  && _running
#pragma omp parallel for schedule(dynamic, 4)
    for (auto y = 0; y < h; ++y) {
        for (auto x = 0; x < w && running_; ++x) {
            const auto ray = camera_.get_ray(x, y);
            const auto pix = compute_pixel(ray, 3);
#pragma omp critical
            {
                image_->setPixel(x, y, pix);
            }
        }
    }
}

glm::dvec3 RayTracer::compute_pixel(const Ray& ray, int max_reflections) const
{
    glm::dvec3 color{0, 0, 0}; // background color

    if (max_reflections <= 0)
        return color;

    glm::dvec3 intersect, normal; // values at minimum
    auto min_ent = scene_->closest_intersection(ray, intersect, normal);

    if (min_ent == nullptr)
        return color;

    // this is the entity material at the intersection location
    const auto mat = min_ent->material; // TODO: use material of sub-entity if explicit_entity

    // this is the base color value of the entity at the intersection location
    const auto color_at_intersect = min_ent->get_color_at_intersect(intersect);

    // compute light and normal vector at the intersection point
    const auto l = glm::normalize(light_ - intersect);

    // check if the light is obstructed by some an entity
    // TODO: this only works for opaque objects
    const auto blocked = scene_->is_blocked(Ray::offset_ray(intersect, l));

    // ambient: L_a = k_a * I_a
    color = color + color_at_intersect * mat->ambient;

    if (blocked)
        return color;

    // diffuse:  L_d = k_d * I * max(0.0, dot(n, l))
    const auto diffuse = glm::dot(normal, l);
    if (diffuse > 0) {
        color += color_at_intersect * mat->diffuse * diffuse;
    }

    // specular (Blinn-Phong): L_s = k_s * I * max(0.0, dot(n,normalize(v + l)))^p
    const auto v = glm::normalize(-ray.dir);     // eye direction
    const auto bisector = glm::normalize(v + l); // center between view and light
    const auto base = glm::dot(normal, bisector);
    if (base > 0) {
        color += mat->specular * glm::pow(base, mat->specular_exponent);
    }

#ifdef USE_MIRROR
    // mirror: L_m = k_m * trace(Ray(P, r))
    if (mat->reflective > 0) {
        // reflection direction: r = 2n * dot(n,v) - v === glm::reflect(-v, n)
        const auto reflect_dir = glm::reflect(-v, normal);
        const auto mirror_ray = Ray::offset_ray(intersect, reflect_dir);
        // Trace another ray in reflection direction
        const auto mirror = compute_pixel(mirror_ray, max_reflections - 1);

        color += mat->reflective * mirror;
    }
#endif
#ifdef USE_REFRACTION
    // glazed: (trace another ray in refraction direction)
    // It is not clear what should happen in the intersection of two objects. Which
    // material / refractive index is used?

    if (mat->refractive > 0) {
        const auto entering = glm::dot(normal, ray.dir);

        // if entering == false -> divide by 1.0 for air
        const auto ref_idx = mat->refractive_index;
        const auto eta = entering ? ray.refractive_index / ref_idx : ref_idx;

        const auto refraction_direction = glm::refract(ray.dir, normal, eta);
        auto refract_ray = Ray::offset_ray(intersect, refraction_direction);

        // reset the index if we leave an object
        refract_ray.refractive_index = entering ? ref_idx : 1.0;

        const auto refraction = compute_pixel(refract_ray, max_reflections - 1);
        color += mat->refractive * refraction;
    }
#endif

    // the color value must be clamped because otherwise high illumination will produce
    // values above 1. This results in errors from Qt.
    color = glm::clamp(color, 0., 1.);

    return color;
}

bool RayTracer::running() const { return running_; }

void RayTracer::stop() { running_ = false; }

void RayTracer::start() { running_ = true; }

std::shared_ptr<Image> RayTracer::get_image() const { return image_; }