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

#include "PathTracer.h"
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

    Gui(int width,
        int height,
        std::shared_ptr<PathTracer> raytracer,
        std::shared_ptr<Scene> scene,
        QWindow* = nullptr);

    ~Gui() override;
};
