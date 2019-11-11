#pragma once

#include "ray.h"
#include <glm/glm.hpp>

/// Represents the camera with information about the 'sensor' size.
struct Camera {
    explicit Camera(glm::dvec3 pos) : Camera(pos, {0, 0, 0}) {}
    Camera(glm::dvec3 pos, glm::dvec3 lookAt) : pos(pos), up({0, 0, 1.0}), forward(lookAt - pos)
    {
        forward = glm::normalize(forward);
    }

    glm::dvec3 pos;
    glm::dvec3 up;
    glm::dvec3 forward;              // normalized vector of the view direction
    const double sensorDiag = 0.035; // diagonal of the sensor
    const double focalDist = 0.04;   // focal distance

    inline Ray getRay(double x, double y, double x_size, double y_size)
    {
        // TODO: right and scale should be cached for better performance

        y = y_size - y; // invert the y axis to match the world coordinate system

        // the side direction can be computed from the up and forward vector since it is
        // perpendicular to both of them
        const auto right = glm::normalize(glm::cross(up, forward));

        // scale is the factor by which the sensor is stretched to match the display size
        const auto scale = sensorDiag / glm::sqrt(x_size * x_size + y_size * y_size);

        // computes the relative location of the pixel in the sensor, i.e. the middle pixel of the
        // screen is in the middle of the sensor
        const auto px = scale * (x + 0.5 - x_size / 2);
        const auto py = scale * (y + 0.5 - y_size / 2);

        // finally compute the direction of the ray which goes through the given pixel.
        // this requires that forward, up and right are normalized
        glm::dvec3 direction = forward * focalDist + up * py + right * px;

        // let rays originate in the camera center
        // TODO: It might be better to originate in the sensor to avoid objects between sensor and
        // eye?
        return {pos, direction};
    }
};
