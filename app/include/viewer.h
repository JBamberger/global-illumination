#pragma once

#include <chrono>
#include <thread>

#include <QCloseEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <utility>

#include "RayTracer.h"
#include "image.h"

class Viewer : public QWidget {
    QTimer* timer_;
    QLabel* duration_text_;
    RayTracer raytracer_;
    std::thread thread_;

  public:
    Viewer(RayTracer raytracer, QLabel* duration_text, QWidget* parent)
        : QWidget(parent), duration_text_(duration_text), raytracer_(std::move(raytracer))
    {
        timer_ = new QTimer(this);
        timer_->setInterval(32);
        timer_->start();
        const auto repaint_callback = [this]() { this->repaint(); };
        connect(timer_, &QTimer::timeout, repaint_callback);
    }

    ~Viewer() { stopRaytrace(); }

    void stopRaytrace()
    {
        if (raytracer_.running()) {
            raytracer_.stop();
            thread_.join();
        }
    }

    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.drawImage(QPoint(0, 0), getImage());
    }

    void resizeEvent(QResizeEvent*) override { restartRaytrace(); }

    QImage getImage() const { return raytracer_.get_image()->_image; }

  private:
    void restartRaytrace()
    {
        if (raytracer_.running()) {
            raytracer_.stop();
            thread_.join();
        }
        raytracer_.start();
        thread_ = std::thread([&]() {
            duration_text_->setText("Running...");
            using namespace std::chrono;
            const auto t1 = high_resolution_clock::now();
            this->raytracer_.run(this->width(), this->height());
            const auto t2 = high_resolution_clock::now();
            const auto duration = duration_cast<milliseconds>(t2 - t1).count();
            duration_text_->setText(QString::number(duration / static_cast<double>(1000)) +
                                    " seconds");
        });
    }
};
