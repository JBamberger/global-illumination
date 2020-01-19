#pragma once
#include "NDChecker.h"
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <utility>

class Texture {
  public:
    virtual ~Texture() = default;
    virtual glm::dvec3 value(glm::dvec2 uv) const = 0;
};

class ConstantTexture final : public Texture {
    glm::dvec3 color_;

  public:
    constexpr explicit ConstantTexture(const glm::dvec3 color) : color_(color) {}

    glm::dvec3 value(glm::dvec2 uv) const override { return color_; }
};

class CheckerboardMaterial final : public Texture {
    const NdChecker<2> checker_;
    const std::shared_ptr<ConstantTexture> color1_;
    const std::shared_ptr<ConstantTexture> color2_;

  public:
    explicit CheckerboardMaterial(const size_t squares = 10,
                                  std::shared_ptr<ConstantTexture> color1 =
                                      std::make_shared<ConstantTexture>(glm::dvec3(0, 0, 0)),
                                  std::shared_ptr<ConstantTexture> color2 =
                                      std::make_shared<ConstantTexture>(glm::dvec3(1, 1, 1)))
        : checker_(squares), color1_(std::move(color1)), color2_(std::move(color2))
    {
    }

    glm::dvec3 value(const glm::dvec2 uv) const override
    {
        return checker_.at({uv.x, uv.y}) ? color2_->value(uv) : color1_->value(uv);
    }
};