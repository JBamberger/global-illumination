#include <QApplication>

#include <iostream>

#include "camera.h"
#include "entities.h"
#include "gui.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    glm::dvec3 a, b;
    Ray r1{{10, 0, 0}, {-1, 0, 0}};

    Ray r2{{10, 0, 0}, {-1, 1, 0}};
    Ray r3{{10, 0, 0}, {-0.04, 0.00401, 0}};
    Ray r4{{10, 0, 0}, {-0.04, 0.00402, 0}};
    Ray r5{{10, 0, 0}, {-0.04, 0.00403, 0}};

    Ray r6{{10, 0, 0}, {-1, -1, 0}};
    Ray r7{{10, 0, 0}, {-0.04, -0.00401, 0}};
    Ray r8{{10, 0, 0}, {-0.04, -0.00402, 0}};
    Ray r9{{10, 0, 0}, {-0.04, -0.00403, 0}};

    Ray r10{{10, 0, 0}, {-1, 0, 1}};
    Ray r11{{10, 0, 0}, {-0.04, 0, 0.00401}};
    Ray r12{{10, 0, 0}, {-0.04, 0, 0.00402}};
    Ray r13{{10, 0, 0}, {-0.04, 0, 0.00403}};

    Ray r14{{10, 0, 0}, {-1, 0, -1}};
    Ray r15{{10, 0, 0}, {-0.04, 0, -0.00401}};
    Ray r16{{10, 0, 0}, {-0.04, 0, -0.00402}};
    Ray r17{{10, 0, 0}, {-0.04, 0, -0.00403}};

    ImplicitSphere s1;
    s1.radius = 1;
    std::cout << s1.intersect(r1, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << std::endl;
    std::cout << s1.intersect(r2, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r3, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r4, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r5, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << std::endl;
    std::cout << s1.intersect(r6, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r7, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r8, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r9, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << std::endl;
    std::cout << s1.intersect(r10, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r11, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r12, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r13, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << std::endl;
    std::cout << s1.intersect(r14, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r15, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r16, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << s1.intersect(r17, a, b) << " " << glm::to_string(a) << std::endl;
    std::cout << std::endl;

    Camera camera({10, 0, 0});
    glm::dvec3 light{10, 10, 10};

    RayTracer raytracer(camera, light);

    // Set up scene
    Octree scene({-20, -20, -20}, {20, 20, 20});
    // TODO Add objects to the scene
    // scene.push_back(...);

    auto* sphere = new ImplicitSphere; // TODO: delete
    sphere->radius = 1;
    scene.push_back(sphere);

    raytracer.setScene(&scene);

    Gui window(500, 500, raytracer);
    window.show();
    return app.exec();
}
