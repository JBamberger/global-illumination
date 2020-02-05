#include "Gui.h"

Gui::Gui(const int width,
         const int height,
         std::shared_ptr<RayTracer> raytracer,
         std::shared_ptr<Scene> scene,
         QWindow*)
{
    duration_text_ = new QLabel(this);
    viewer_ = new Viewer(std::move(raytracer), std::move(scene), duration_text_, this);
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

    samplesMenu_ = menuBar()->addMenu(tr("Sample &count"));
    std::array<size_t, 12> sizes = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
    for (auto size : sizes) {
        std::string num = std::to_string(size);
        const auto action = new QAction(tr(num.c_str()), this);
        action->setStatusTip(tr(("Set the sample size to " + num + ".").c_str()));
        connect(action, &QAction::triggered, this,
                std::bind(&Viewer::setSampleCount, viewer_, size));

        samplesMenu_->addAction(action);
    }

    struct SceneMenuEntry {
        const char* title;
        const char* status_tip;
        SceneSetting scene;
    };

    std::array<SceneMenuEntry, 5> scenes = {
        SceneMenuEntry{"Empty Box", "Empty Cornell box.", SceneSetting::Empty},
        SceneMenuEntry{"Cornell Box", "Cornell box with one cube and two spheres.",
                       SceneSetting::Cornell},
        SceneMenuEntry{"Pig", "Pig model consisting of multiple parts.", SceneSetting::Pig},
        SceneMenuEntry{"Spot (cow)", "Cow model with image texture.", SceneSetting::Cow},
        SceneMenuEntry{"Stanford dragon", "Stanford dragon model with many primitives.",
                       SceneSetting::Dragon}};

    sceneMenu_ = menuBar()->addMenu(tr("&Scene"));
    for (const auto& s : scenes) {
        const auto action = new QAction(tr(s.title), this);
        action->setStatusTip(tr(s.status_tip));
        connect(action, &QAction::triggered, this, std::bind(&Viewer::setScene, viewer_, s.scene));
        sceneMenu_->addAction(action);
    }

    this->resize(width, height);
}

Gui::~Gui() { viewer_->stopRaytrace(); }
