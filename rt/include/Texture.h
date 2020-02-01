#pragma once
#include "NDChecker.h"
#include <QtGui/QImage>
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

class ImageBackedTexture final : public Texture {
    size_t width;
    size_t height;
    std::vector<glm::dvec3> image;

  public:
    explicit ImageBackedTexture(const std::string& name) : width(0), height(0)
    {
        QImage img;
        if (!img.load(QString::fromStdString(name))) {
            std::cerr << "Could not load texture at location " << name << "." << std::endl;
            throw std::runtime_error("Could not load texture at location " + name + ".");
        }
        width = img.size().width();
        height = img.size().height();
        image.reserve(width * height);

        for (int y = static_cast<int>(height - 1); y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                const auto p = img.pixel(x, y);
                image.emplace_back(qRed(p) / 255., qGreen(p) / 255., qBlue(p) / 255.);
            }
        }
    }
    glm::dvec3 value(glm::dvec2 uv) const override
    {
        const auto x = lround(uv.x * width);
        const auto y = lround(uv.y * height);
        return image[y * width + x];
    }
};