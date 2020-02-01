#pragma once

#include "Entity.h"
#include <vector>

namespace obj {

using ObjContent = std::vector<Triangle>;

ObjContent makeSphere(glm::dvec3 center = {0, 0, 0},
                      double radius = 1,
                      int sub_divisions = 2,
                      bool use_tetrahedron = false);
ObjContent makeQuad(glm::dvec3 a, glm::dvec3 b, glm::dvec3 c, glm::dvec3 d);
ObjContent makeCube(glm::dvec3 center, double side_length);
ObjContent makeCuboid(glm::dvec3 center, glm::dvec3 size);
ObjContent makeOctet(std::array<glm::dvec3, 8> corners);
ObjContent makeCone(glm::dvec3 center, glm::dvec3 tip, double radius, size_t slices);

std::ostream& operator<<(std::ostream& os, const ObjContent& content);
std::istream& operator>>(std::istream& is, ObjContent& content);

ObjContent rotate_x(ObjContent content, double angle);
ObjContent rotate_y(ObjContent content, double angle);
ObjContent rotate_z(ObjContent content, double angle);
ObjContent translate(ObjContent content, glm::dvec3 delta);
ObjContent center(ObjContent content);
ObjContent scale(ObjContent content, double scale);
ObjContent scale(ObjContent content, glm::dvec3 scale);

BoundingBox computeBBox(const ObjContent& content);

} // namespace obj