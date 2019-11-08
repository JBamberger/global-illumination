#pragma once

#include "ray.h"
#include <glm/glm.hpp>

/// Represents the camera with information about the 'sensor' size.
struct Camera {
    explicit Camera(glm::dvec3 pos) : Camera(pos, {0, 0, 0}) {}
    Camera(glm::dvec3 pos, glm::dvec3 lookAt) : pos(pos), up({0, 0, 1.0}), forward(lookAt - pos) {
        forward = glm::normalize(forward);
    }

    glm::dvec3 pos;
    glm::dvec3 up;
    glm::dvec3 forward;              // normalized vector of the view direction
    const double sensorDiag = 0.035; // diagonal of the sensor
    const double focalDist = 0.04;   // focal distance

    inline Ray getRay(double x, double y, double x_size, double y_size) {
        glm::dvec3 origin;
        glm::dvec3 direction;

        double scale = sensorDiag / glm::sqrt(x_size * x_size + y_size * y_size);

        double w = scale * x_size;
        double h = scale * y_size;

        double px = x * scale - (w / 2);
        double py = y * scale - (h / 2);

        glm::dvec3 right = glm::normalize(glm::cross(up, forward));
        glm::dvec3 fwd = forward * focalDist;
        glm::dvec3 u = up * py;
        glm::dvec3 r = right * px;
        origin = pos;
        direction = fwd  // move to image plain
                    + u  // move up
                    + r; // move right

        return {origin, direction};
    }
};
