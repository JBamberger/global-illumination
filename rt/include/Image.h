#pragma once

#include <QImage>

#include <glm/glm.hpp>

class Image {
    QImage _image;

  public:
    Image() = delete;

    Image(int width, int height);

    [[nodiscard]] int width() const;

    [[nodiscard]] int height() const;

    void setPixel(int x, int y, glm::dvec3 c);

    [[nodiscard]] glm::dvec3 getPixel(int x, int y) const;

    void clear();

  private:
    friend class Viewer;
};
