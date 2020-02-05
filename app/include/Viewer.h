#pragma once

#include <chrono>
#include <thread>

#include <QCloseEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <utility>

#include "Image.h"
#include "RayTracer.h"

class Viewer : public QWidget {
    QTimer* timer_;
    QLabel* duration_text_;
    std::shared_ptr<RayTracer> raytracer_;
    std::shared_ptr<Scene> scene_;
    std::thread thread_;

  public:
    Viewer(std::shared_ptr<RayTracer> raytracer,
           std::shared_ptr<Scene> scene,
           QLabel* duration_text,
           QWidget* parent);

    ~Viewer() override;

    void setSampleCount(size_t samples);

    void setScene(const SceneSetting setting);

    void startRaytrace();

    void stopRaytrace();

    void paintEvent(QPaintEvent*) override;

    void resizeEvent(QResizeEvent*) override;

    QImage getImage() const;

  private:
    void restartRaytrace();
};
