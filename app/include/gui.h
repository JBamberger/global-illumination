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
#include "viewer.h"

class Gui : public QMainWindow {
    QLabel* duration_text_;
    //    QPushButton* save_button_;
    Viewer* viewer_;
    QMenu* fileMenu_;
    QMenu* samplesMenu_;
    QAction* saveAction_;

  public:
    Gui() = delete;

    Gui(const int width, const int height, RayTracer raytracer, QWindow* = nullptr)
    {

        //        auto toolbar = new QToolBar(this);
        //        toolbar->setMovable(false);
        //
        //        save_button_ = new QPushButton("Save as ...", this);
        //        toolbar->addWidget(save_button_);
        //
        //        auto spacer = new QWidget();
        //        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //        toolbar->addWidget(spacer);

        duration_text_ = new QLabel(this);
        //        toolbar->addWidget(duration_text_);
        //        this->addToolBar(toolbar);

        viewer_ = new Viewer(std::move(raytracer), duration_text_, this);
        this->setCentralWidget(viewer_);

        statusBar()->insertPermanentWidget(0, duration_text_);

        const auto save_callback = [this]() {
            const auto filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "render.png",
                                                               tr("Images (*.png);;All Files (*)"));
            if (filename == nullptr || filename.isEmpty()) {
                std::cerr << "No file selected." << std::endl;
            } else {
                QFile file(filename);
                file.open(QIODevice::WriteOnly);
                viewer_->getImage().save(&file, "PNG");
            }
        };

        saveAction_ = new QAction(tr("&Save as ..."), this);
        saveAction_->setShortcut(tr("Ctrl+S"));
        saveAction_->setStatusTip(tr("Save the file with a new name."));
        connect(saveAction_, &QAction::triggered, this, save_callback);

        fileMenu_ = menuBar()->addMenu(tr("&File"));
        fileMenu_->addAction(saveAction_);

        samplesMenu_ = menuBar()->addMenu(tr("&Sample count"));
        std::array<size_t, 12> sizes = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
        for (auto size : sizes) {
            std::string num = std::to_string(size);
            const auto action = new QAction(tr(num.c_str()), this);
            action->setStatusTip(tr(("Set the sample size to " + num + ".").c_str()));
            connect(action, &QAction::triggered, this,
                    std::bind(&Viewer::setSampleCount, viewer_, size));

            samplesMenu_->addAction(action);
        }

        //        connect(save_button_, &QPushButton::clicked, save_callback);

        this->resize(width, height);
    }

    ~Gui() override { viewer_->stopRaytrace(); }
};
