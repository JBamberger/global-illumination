#pragma once

#include <chrono>
#include <thread>

#include <QCloseEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <utility>

#include "image.h"
#include "raytracer.h"

class Viewer : public QWidget {
  public:
    Viewer(RayTracer raytracer, QLabel* durationText, QWidget* parent)
        : QWidget(parent), _durationText(durationText), _raytracer(std::move(raytracer))
    {
        _timer = new QTimer(this);
        _timer->setInterval(32);
        _timer->start();
        connect(_timer, &QTimer::timeout, [this]() { this->repaint(); });
        restart_raytrace();
    }

    ~Viewer() { stop_raytrace(); }

    void stop_raytrace()
    {
        if (_raytracer.running()) {
            _raytracer.stop();
            _thread.join();
        }
    }

    void paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        painter.drawImage(QPoint(0, 0), getImage());
    }

    void resizeEvent(QResizeEvent*) { restart_raytrace(); }

    QImage getImage() const { return _raytracer.get_image()->_image; }

  private:
    void restart_raytrace()
    {
        if (_raytracer.running()) {
            _raytracer.stop();
            _thread.join();
        }
        _raytracer.start();
        _thread = std::thread([&]() {
            _durationText->setText("Running...");
            using namespace std::chrono;
            const auto t1 = high_resolution_clock::now();
            this->_raytracer.run(this->width(), this->height());
            const auto t2 = high_resolution_clock::now();
            const auto duration = duration_cast<milliseconds>(t2 - t1).count();
            _durationText->setText(QString::number(duration / static_cast<double>(1000)) +
                                   " seconds");
        });
    }

    QTimer* _timer;
    QLabel* _durationText;
    RayTracer _raytracer;
    std::thread _thread;
};
