/**
 *    Copyright 2020 Jannik Bamberger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Viewer.h"

Viewer::Viewer(std::shared_ptr<PathTracer> raytracer,
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

Viewer::~Viewer() { stopRaytrace(); }

void Viewer::setSampleCount(size_t samples)
{
    stopRaytrace();
    raytracer_->setSampleCount(samples);
    startRaytrace();
}

void Viewer::setScene(const SceneSetting setting)
{
    stopRaytrace();
    scene_->useSceneSetting(setting);
    raytracer_->setScene(scene_->getTree());
    startRaytrace();
}

void Viewer::startRaytrace()
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
        duration_text_->setText(QString::number(duration / static_cast<double>(1000)) + " seconds");
    });
}

void Viewer::stopRaytrace()
{
    if (raytracer_->running()) {
        raytracer_->stop();
        thread_.join();
    }
}

void Viewer::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), getImage());
}

void Viewer::resizeEvent(QResizeEvent*) { restartRaytrace(); }

QImage Viewer::getImage() const { return raytracer_->getImage()->_image; }

void Viewer::restartRaytrace()
{
    stopRaytrace();
    startRaytrace();
}
