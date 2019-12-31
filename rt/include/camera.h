#pragma once

#include "ray.h"
#include <glm/glm.hpp>

/// Represents the camera with information about the 'sensor' size.
class Camera {
    /// Location of the camera focus point.
    glm::dvec3 pos_;

    /// Normalized vector pointing upwards.
    glm::dvec3 up_;
    /// Normalized vector pointing to the right.
    glm::dvec3 right_;
    /// Normalized vector pointing in viewing direction.
    glm::dvec3 forward_;

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
    Camera(glm::dvec3 pos, glm::dvec3 look_at)
        : pos_(pos), up_({0, 0, 1.0}), forward_(look_at - pos)
    {
        forward_ = glm::normalize(forward_);
        up_ = glm::normalize(up_);
        // The side direction can be computed from the up and forward vector since it is
        // perpendicular to both of them. The value is negated, because the sensor points in the
        // other direction than the viewer sees the image.
        right_ = -glm::normalize(glm::cross(up_, forward_));
    }

    Ray get_ray(const double x, const double y) const
    {
        // computes the relative location of the pixel in the sensor, i.e. the middle pixel of the
        // screen is in the middle of the sensor
        const auto px = window_scale_ * (x + 0.5 - window_width_ / 2);
        // for the y location we also need to invert the input axis, as the qt image points y
        // downwards and we point y upwards
        const auto py = window_scale_ * (windows_height_ / 2 - y + 0.5);

        // Finally compute the direction of the ray which goes through the given pixel.
        // this requires that forward, up and right are normalized.
        const auto direction = forward_ * focal_dist_ + up_ * py + right_ * px;

        // let rays originate in the camera center
        // TODO: It might be better to originate in the sensor to avoid objects between sensor and
        // eye?
        return {pos_, direction};
    }

    void set_window_size(const double w, const double h)
    {
        window_width_ = w;
        windows_height_ = h;
        window_scale_ = sensor_diag_ / glm::sqrt(w * w + h * h);
    }
};
