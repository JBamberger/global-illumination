/**
 *    Copyright 2020 Jannik Bamberger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Camera.h"

Camera::Camera(const glm::dvec3 pos) : Camera(pos, {0, 0, 0}) {}

Camera::Camera(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 v_up)
    : pos_(pos), w_(normalize(pos - look_at)), u_(normalize(glm::cross(v_up, w_))),
      v_(normalize(glm::cross(w_, u_)))
{
}

Ray Camera::getRay(const double x, const double y) const
{
    // apply some jitter to the input pixel position
    const auto dx = rng();
    const auto dy = rng();

    // computes the relative location of the pixel in the sensor, i.e. the middle pixel of the
    // screen is in the middle of the sensor
    const auto px = window_scale_ * (x + dx - window_width_ / 2);
    // for the y location we also need to invert the input axis, as the qt image points y
    // downwards and we point y upwards
    const auto py = window_scale_ * (windows_height_ / 2 - y + dy);

    // Finally compute the direction of the ray which goes through the given pixel.
    // this requires that forward, up and right are normalized.
    const auto direction = -w_ * focal_dist_ + v_ * py + u_ * px;

    // let rays originate in the camera center
    // TODO: It might be better to originate in the sensor to avoid objects between sensor and
    // eye?
    return Ray(pos_, direction);
}

void Camera::setWindowSize(const double w, const double h)
{
    window_width_ = w;
    windows_height_ = h;
    window_scale_ = sensor_diag_ / glm::sqrt(w * w + h * h);
}
