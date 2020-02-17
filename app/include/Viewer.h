#pragma once

#include <chrono>
#include <thread>

#include "Image.h"
#include "PathTracer.h"
#include "Scene.h"
#include <QCloseEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <QtWidgets/QLabel>
#include <utility>

/**
 * Qt widget that hosts the ray/path tracing in a separate thread and visualizes the result.
 */
class Viewer : public QWidget {

    /**
     * Repaint timer.
     */
    QTimer* timer_;

    /**
     * Label to display the rendering time.
     */
    QLabel* duration_text_;

    /**
     * Tracer object.
     */
    std::shared_ptr<PathTracer> raytracer_;

    /**
     * Currently active scene.
     */
    std::shared_ptr<Scene> scene_;

    /**
     * Tracing thread.
     */
    std::thread thread_;

  public:
    Viewer(std::shared_ptr<PathTracer> raytracer,
           std::shared_ptr<Scene> scene,
           QLabel* duration_text,
           QWidget* parent);

    ~Viewer() override;

    /**
     * Changes the sample count per pixel and restarts the tracing.
     * @param samples
     */
    void setSampleCount(size_t samples);

    /**
     * Changes the scene and restarts the tracing.
     * @param setting scene specification
     */
    void setScene(SceneSetting setting);

    /**
     * Starts the raytracing thread.
     */
    void startRaytrace();

    /**
     * Signals the tracing thread and then waits for it to join. Returns afterwards.
     */
    void stopRaytrace();

    void paintEvent(QPaintEvent*) override;

    void resizeEvent(QResizeEvent*) override;

    /**
     * Returns the tracing image buffer.
     * @return image buffer
     */
    [[nodiscard]] QImage getImage() const;

  private:
    /**
     * Stops and starts the tracing as described in stopRaytrace and startRaytrace.
     */
    void restartRaytrace();
};
