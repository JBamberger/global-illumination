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

#include "Camera.h"
#include "Gui.h"
#include "Material.h"
#include "Scene.h"
#include <QApplication>
#include <QCommandLineParser>
#include <filesystem>
#include <iostream>
#include <memory>

constexpr const char* app_name = "PathTracer";
constexpr const char* app_version = "v1.0.0";
constexpr const char* app_description = "PathTracer written for Global Illumination Methods WS1920";

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(app_name);
    QApplication::setApplicationVersion(app_version);

    // QCommandLineParser to read cli args
    QCommandLineParser parser;
    parser.setApplicationDescription(app_description);
    const auto help_option = parser.addHelpOption();
    const auto version_option = parser.addVersionOption();
    parser.addPositionalArgument("share_dir",
                                 "Directory containing the share files (objects, textures, ...).");
    parser.process(app);

    if (parser.isSet(help_option)) {
        parser.showHelp();
    }
    if (parser.isSet(version_option)) {
        parser.showVersion();
    }

    std::filesystem::path share_dir = "./share";
    const auto pa = parser.positionalArguments();
    if (!pa.isEmpty()) {
        share_dir = parser.positionalArguments()[0].toStdString();
    }
    if (!std::filesystem::exists(share_dir)) {
        std::cerr << "Share directory does not exist." << std::endl;
        parser.showHelp(EXIT_FAILURE);
    }

    std::cout << "ShareDir: " << share_dir << std::endl;

    Camera camera(glm::dvec3{14, 0, 0});

    // scene setup
    auto scene =
        std::make_shared<Scene>(share_dir, glm::dvec3{-20, -20, -20}, glm::dvec3{20, 20, 20});
    scene->addCornellBox().addCornellContent();

    auto raytracer = std::make_shared<PathTracer>(camera, scene->getTree());

    Gui window(500, 500, std::move(raytracer), std::move(scene));
    window.show();
    return QApplication::exec();
}
