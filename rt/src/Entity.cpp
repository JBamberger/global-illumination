#include "Entity.h"
#include "Material.h"

Entity::Entity()
    : material_(std::make_shared<LambertianMaterial>(
          std::make_shared<ConstantTexture>(glm::dvec3{1, 0, 0})))
{
}
