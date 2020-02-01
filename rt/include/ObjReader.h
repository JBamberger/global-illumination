#pragma once

#include "BVH.h"
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

BoundingBox computeBBox(const ObjContent& content);

class Transform {
    class Step;
    class Translate;
    class Scale;
    class Center;
    class Rotate;
    std::vector<std::unique_ptr<Step>> transforms_;

  public:
    Transform& rotate_x(double angle);
    Transform& rotate_y(double angle);
    Transform& rotate_z(double angle);
    Transform& center();
    Transform& translate(glm::dvec3 delta);
    Transform& scale(double scale);
    Transform& scale(glm::dvec3 scale);

    ObjContent apply(ObjContent content) const;
    std::unique_ptr<BVH> to_bvh(ObjContent content) const;

  private:
    class Step {
        virtual void pre(const ObjContent& content) {}
        virtual void process(Triangle& t) = 0;

        friend ObjContent Transform::apply(ObjContent content) const;
    };
};

} // namespace obj