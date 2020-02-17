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

#include "BoundingBox.h"

#include <cassert>

BoundingBox::BoundingBox(const glm::dvec3 min, const glm::dvec3 max) : min(min), max(max)
{
    assert(min.x <= max.x);
    assert(min.y <= max.y);
    assert(min.z <= max.z);
}

bool BoundingBox::intersect(const BoundingBox& other) const
{
    // Check if the intervals in each dimension overlap (slab intersection)
    return (min.x <= other.max.x && other.min.x <= max.x) && // test x dimension
           (min.y <= other.max.y && other.min.y <= max.y) && // test y dimension
           (min.z <= other.max.z && other.min.z <= max.z);   // test z dimension
}

bool BoundingBox::intersect(const Ray& ray) const
{
    // compute the intersection with each slab given by the box
    const auto mins = (min - ray.origin) / ray.dir;
    const auto maxes = (max - ray.origin) / ray.dir;

    // build valid intervals
    const auto t_mins = glm::min(mins, maxes);
    const auto t_maxes = glm::max(mins, maxes);

    // Intersect first and second interval
    auto t_min = glm::max(t_mins.x, t_mins.y);
    auto t_max = glm::min(t_maxes.x, t_maxes.y);
    if (t_min > t_max)
        return false;

    // Intersect remaining interval with result
    t_min = glm::max(t_min, t_mins.z);
    t_max = glm::min(t_max, t_maxes.z);
    if (t_min > t_max)
        return false;

    // check that the intersection is not behind the ray starting point
    return t_max > 0;
}

bool BoundingBox::contains(glm::dvec3 point) const
{
    // basically the same as the bbox test but min and max are the same values
    return (min.x <= point.x && point.x <= max.x) && // test x dimension
           (min.y <= point.y && point.y <= max.y) && // test y dimension
           (min.z <= point.z && point.z <= max.z);   // test z dimension
}

BoundingBox BoundingBox::unite(const BoundingBox& b1, const BoundingBox& b2)
{
    return BoundingBox{glm::min(b1.min, b2.min), glm::max(b1.max, b2.max)};
}
