#include "Image.h"

Image::Image(int width, int height) : _image(width, height, QImage::Format_RGB888)
{
    this->clear();
}

int Image::width() const { return _image.width(); }

int Image::height() const { return _image.height(); }

void Image::setPixel(int x, int y, glm::dvec3 c)
{
    _image.setPixel(x, y,
                    QColor(static_cast<int>(255 * c.r), static_cast<int>(255 * c.g),
                           static_cast<int>(255 * c.b))
                        .rgb());
}

glm::dvec3 Image::getPixel(int x, int y) const
{
    const auto p = _image.pixel(x, y);
    return {qRed(p) / 255., qGreen(p) / 255., qBlue(p) / 255.};
}

void Image::clear() { _image.fill(Qt::black); }
