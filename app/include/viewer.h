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
           QWidget* parent)
        : QWidget(parent), duration_text_(duration_text), raytracer_(std::move(raytracer)),
          scene_(std::move(scene))
    {
        timer_ = new QTimer(this);
        timer_->setInterval(32);
        timer_->start();
        const auto repaint_callback = [this]() { this->repaint(); };
        connect(timer_, &QTimer::timeout, repaint_callback);
    }

    ~Viewer() override { stopRaytrace(); }

    void setSampleCount(size_t samples)
    {
        stopRaytrace();
        raytracer_->setSampleCount(samples);
        startRaytrace();
    }

    void setScene(const SceneSetting setting)
    {
        stopRaytrace();
        scene_->useSceneSetting(setting);
        raytracer_->setScene(scene_->getTree());
        startRaytrace();
    }

    void startRaytrace()
    {
        if (raytracer_->running()) {
            std::cerr << "Tried to start raytracer that is running already." << std::endl;
            throw std::runtime_error("Raytracer is already running.");
        }

        raytracer_->start();
        thread_ = std::thread([&]() {
            using namespace std::chrono;

            duration_text_->setText("Running...");
            const auto t1 = high_resolution_clock::now();
            raytracer_->run(this->width(), this->height());
            const auto t2 = high_resolution_clock::now();
            const auto duration = duration_cast<milliseconds>(t2 - t1).count();
            duration_text_->setText(QString::number(duration / static_cast<double>(1000)) +
                                    " seconds");
        });
    }

    void stopRaytrace()
    {
        if (raytracer_->running()) {
            raytracer_->stop();
            thread_.join();
        }
    }

    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.drawImage(QPoint(0, 0), getImage());
    }

    void resizeEvent(QResizeEvent*) override { restartRaytrace(); }

    QImage getImage() const { return raytracer_->getImage()->_image; }

  private:
    void restartRaytrace()
    {
        stopRaytrace();
        startRaytrace();
    }
};
