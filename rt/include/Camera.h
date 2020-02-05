#pragma once

#include "RandomUtils.h"
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
    explicit Camera(glm::dvec3 pos);

    Camera(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 v_up = {0, 0, 1});

    [[nodiscard]] Ray getRay(double x, double y) const;

    void setWindowSize(double w, double h);
};
