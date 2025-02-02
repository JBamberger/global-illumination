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

#include "entities.h"

#include "ObjReader.h"

std::unique_ptr<ExplicitEntity> entities::makeQuad(glm::dvec3 a,
                                                   glm::dvec3 b,
                                                   glm::dvec3 c,
                                                   glm::dvec3 d)
{
    return std::make_unique<ExplicitEntity>(obj::makeQuad(a, b, c, d));
}

std::unique_ptr<ExplicitEntity> entities::makeOctet(std::array<glm::dvec3, 8> corners)
{
    return std::make_unique<ExplicitEntity>(obj::makeOctet(corners));
}

std::unique_ptr<ExplicitEntity> entities::makeCube(glm::dvec3 center, double side_length)
{
    return std::make_unique<ExplicitEntity>(obj::makeCube(center, side_length));
}

std::unique_ptr<ExplicitEntity> entities::makeCuboid(glm::dvec3 center, glm::dvec3 size)
{
    return std::make_unique<ExplicitEntity>(obj::makeCuboid(center, size));
}

std::unique_ptr<ExplicitEntity> entities::makeSphere(const glm::dvec3 center,
                                                     const double radius,
                                                     const int sub_divisions,
                                                     const bool use_tetrahedron)
{
    return std::make_unique<ExplicitEntity>(
        obj::makeSphere(center, radius, sub_divisions, use_tetrahedron));
}

std::unique_ptr<ExplicitEntity> entities::makeCone(const glm::dvec3 center,
                                                   const glm::dvec3 tip,
                                                   const double radius,
                                                   const size_t slices)
{
    return std::make_unique<ExplicitEntity>(obj::makeCone(center, tip, radius, slices));
}
