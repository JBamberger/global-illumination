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

/**
 * Host window of the ray tracing application.
 */
class Gui : public QMainWindow {
    /**
     * Viewer widget that hosts the ray tracer.
     */
    Viewer* viewer_;

  public:
    Gui() = delete;

    /**
     * Creates a new instance of the main window.
     * @param width window width
     * @param height window height
     * @param raytracer raytracer object
     * @param scene scene object
     */
    Gui(int width,
        int height,
        std::shared_ptr<PathTracer> raytracer,
        std::shared_ptr<Scene> scene,
        QWindow* = nullptr);

    ~Gui() override;
};
