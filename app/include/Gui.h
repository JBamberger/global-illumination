#pragma once

#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QStatusBar>
#include <QToolBar>
#include <utility>

#include "RayTracer.h"
#include "Scene.h"
#include "Viewer.h"

class Gui : public QMainWindow {

    QLabel* duration_text_;
    Viewer* viewer_;
    QMenu* fileMenu_;
    QMenu* samplesMenu_;
    QMenu* sceneMenu_;
    QAction* saveAction_;

  public:
    Gui() = delete;

    Gui(const int width,
        const int height,
        std::shared_ptr<RayTracer> raytracer,
        std::shared_ptr<Scene> scene,
        QWindow* = nullptr);

    ~Gui() override;
};
