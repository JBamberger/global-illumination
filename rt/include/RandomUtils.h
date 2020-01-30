#pragma once
#include <chrono>
#include <glm/glm.hpp>
#include <random>

inline double rng()
{
    static thread_local std::default_random_engine engine(
        std::chrono::system_clock::now().time_since_epoch().count());

    std::uniform_real_distribution<double> dist(0, 1);
    return dist(engine);
}

inline glm::dvec3 randomOffset()
{
    //    glm::dvec3 p;
    //    do {
    //        p = glm::dvec3(rng(), rng(), rng()) * 2.0 - glm::dvec3(1, 1, 1);
    //    } while (glm::dot(p, p) >= 1.0);
    //    return p;
    glm::dvec3 vec;
    auto r1 = rng();
    auto r2 = rng();
    vec.x = glm::cos(glm::two_pi<double>() * r1) * 2 * glm::sqrt(r2 * (1 - r2));
    vec.y = glm::sin(glm::two_pi<double>() * r1) * 2 * glm::sqrt(r2 * (1 - r2));
    vec.z = 1 - 2 * r2;
    return vec;
}
