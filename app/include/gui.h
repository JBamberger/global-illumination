#pragma once

#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include <utility>

#include "RayTracer.h"
#include "viewer.h"

class Gui : public QMainWindow {
  public:
    Gui() = delete;

    Gui(const int width, const int height, RayTracer raytracer, QWindow* parent = nullptr)
    {
        auto toolbar = new QToolBar(this);
        toolbar->setMovable(false);

        save_button_ = new QPushButton("Save as ...", this);
        toolbar->addWidget(save_button_);

        auto spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolbar->addWidget(spacer);

        duration_text_ = new QLabel(this);
        toolbar->addWidget(duration_text_);
        this->addToolBar(toolbar);

        viewer_ = new Viewer(std::move(raytracer), duration_text_, this);
        viewer_->resize(width, height);
        this->setCentralWidget(viewer_);

        connect(save_button_, &QPushButton::clicked, [this]() {
            const auto filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "render.png",
                                                               tr("Images (*.png);;All Files (*)"));
            QFile file(filename);
            file.open(QIODevice::WriteOnly);
            viewer_->getImage().save(&file, "PNG");
        });

        this->resize(width, height);
    }

    ~Gui() { viewer_->stopRaytrace(); }

  private:
    QLabel* duration_text_;
    QPushButton* save_button_;
    Viewer* viewer_;
};
