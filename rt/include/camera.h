#pragma once

#include "Ray.h"
#include <glm/glm.hpp>

/// Represents the camera with information about the 'sensor' size.
class Camera {
    /// Location of the camera focus point.
    glm::dvec3 pos_;

    /// Normalized vector pointing from sensor to focus point.
    glm::dvec3 w_;
    /// Normalized vector pointing to the right.
    glm::dvec3 u_;
    /// Normalized vector pointing upwards.
    glm::dvec3 v_;

    /// Diagonal of the sensor
    const double sensor_diag_ = 0.035;

    /// Focal distance of the camera
    const double focal_dist_ = 0.04;

    /// Window width.
    double window_width_ = 0;
    /// Window height.
    double windows_height_ = 0;
    /// The factor by which the sensor is stretched to match the display size.
    double window_scale_ = 0.0;

  public:
    explicit Camera(const glm::dvec3 pos) : Camera(pos, {0, 0, 0}) {}

    Camera(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 v_up = {0, 0, 1})
        : pos_(pos), w_(normalize(pos - look_at)), u_(normalize(glm::cross(v_up, w_))),
          v_(normalize(glm::cross(w_, u_)))
    {
    }

    Ray getRay(const double x, const double y) const
    {
        // computes the relative location of the pixel in the sensor, i.e. the middle pixel of the
        // screen is in the middle of the sensor
        const auto px = window_scale_ * (x + 0.5 - window_width_ / 2);
        // for the y location we also need to invert the input axis, as the qt image points y
        // downwards and we point y upwards
        const auto py = window_scale_ * (windows_height_ / 2 - y + 0.5);

        // Finally compute the direction of the ray which goes through the given pixel.
        // this requires that forward, up and right are normalized.
        const auto direction = -w_ * focal_dist_ + v_ * py + u_ * px;

        // let rays originate in the camera center
        // TODO: It might be better to originate in the sensor to avoid objects between sensor and
        // eye?
        return Ray(pos_, direction);
    }

    void setWindowSize(const double w, const double h)
    {
        window_width_ = w;
        windows_height_ = h;
        window_scale_ = sensor_diag_ / glm::sqrt(w * w + h * h);
    }
};
