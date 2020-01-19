#pragma once
#include "NDChecker.h"
#include <array>
#include <cassert>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Texture {
  public:
    virtual ~Texture() = default;
    virtual glm::dvec3 value(glm::dvec2 uv) = 0;
};

struct ConstantTexture final : Texture {
    constexpr explicit ConstantTexture(const glm::dvec3 color) : color(color) {}

    glm::dvec3 value(glm::dvec2 uv) override { return color; }

    glm::dvec3 color;
};

class CheckerboardMaterial final : public Texture {
    const NdChecker<2> checker_;
    const std::shared_ptr<ConstantTexture> color1_;
    const std::shared_ptr<ConstantTexture> color2_;

  public:
    explicit CheckerboardMaterial(const size_t squares = 10,
                                  const std::shared_ptr<ConstantTexture> color1 =
                                      std::make_shared<ConstantTexture>(glm::dvec3(0, 0, 0)),
                                  const std::shared_ptr<ConstantTexture> color2 =
                                      std::make_shared<ConstantTexture>(glm::dvec3(1, 1, 1)))
        : checker_(squares), color1_(color1), color2_(color2)
    {
    }

    glm::dvec3 value(const glm::dvec2 uv) override
    {
        return checker_.at({uv.x, uv.y}) ? color2_->value(uv) : color1_->value(uv);
    }
};