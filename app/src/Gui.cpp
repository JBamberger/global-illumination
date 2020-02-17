#include "Gui.h"

Gui::Gui(const int width,
         const int height,
         std::shared_ptr<PathTracer> raytracer,
         std::shared_ptr<Scene> scene,
         QWindow*)
{
    auto* duration_text = new QLabel(this);
    viewer_ = new Viewer(std::move(raytracer), std::move(scene), duration_text, this);
    this->setCentralWidget(viewer_);

    statusBar()->insertPermanentWidget(0, duration_text);

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

    auto* save_action = new QAction(tr("&Save as ..."), this);
    save_action->setShortcut(tr("Ctrl+S"));
    save_action->setStatusTip(tr("Save the file with a new name."));
    connect(save_action, &QAction::triggered, this, save_callback);

    auto* file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(save_action);

    auto* samples_menu = menuBar()->addMenu(tr("Sample &count"));
    std::array<size_t, 12> sizes = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384};
    for (auto size : sizes) {
        std::string num = std::to_string(size);
        const auto action = new QAction(tr(num.c_str()), this);
        action->setStatusTip(tr(("Set the sample size to " + num + ".").c_str()));
        connect(action, &QAction::triggered, this,
                std::bind(&Viewer::setSampleCount, viewer_, size));

        samples_menu->addAction(action);
    }

    struct SceneMenuEntry {
        const char* title;
        const char* status_tip;
        SceneSetting scene;
    };

    std::array<SceneMenuEntry, 6> scenes = {
        SceneMenuEntry{"Empty Box", "Empty Cornell box.", SceneSetting::Empty},
        SceneMenuEntry{"Cornell Box", "Cornell box with one cube and two spheres.",
                       SceneSetting::Cornell},
        SceneMenuEntry{"Exam Box", "Scene for the exam image", SceneSetting::Exam},
        SceneMenuEntry{"Pig", "Pig model consisting of multiple parts.", SceneSetting::Pig},
        SceneMenuEntry{"Spot (cow)", "Cow model with image texture.", SceneSetting::Cow},
        SceneMenuEntry{"Stanford dragon", "Stanford dragon model with many primitives.",
                       SceneSetting::Dragon}};

    auto* scene_menu = menuBar()->addMenu(tr("&Scene"));
    for (const auto& s : scenes) {
        const auto action = new QAction(tr(s.title), this);
        action->setStatusTip(tr(s.status_tip));
        connect(action, &QAction::triggered, this, std::bind(&Viewer::setScene, viewer_, s.scene));
        scene_menu->addAction(action);
    }

    this->resize(width, height);
}

Gui::~Gui() { viewer_->stopRaytrace(); }
