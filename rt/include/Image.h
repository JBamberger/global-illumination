#pragma once

#include <QImage>

#include <glm/glm.hpp>

/**
 * Wrapper class for a QImage.
 */
class Image {
    QImage _image;

  public:
    Image() = delete;

    /**
     * Creates a black image with the given dimensions.
     * @param width image width
     * @param height image height
     */
    Image(int width, int height);

    /**
     * Returns the image width.
     */
    [[nodiscard]] int width() const;

    /**
     * Returns the image height.
     */
    [[nodiscard]] int height() const;

    /**
     * Changes the given pixel color to the provided color.
     * @param x pixel x-position
     * @param y pixel y-position
     * @param c new pixel color
     */
    void setPixel(int x, int y, glm::dvec3 c);

    /**
     * Returns the pixel color at the given position.
     * @param x pixel x-position
     * @param y pixel y-position
     * @return pixel color
     */
    [[nodiscard]] glm::dvec3 getPixel(int x, int y) const;

    /**
     * Sets all image pixels to black.
     */
    void clear();

  private:
    friend class Viewer;
};
