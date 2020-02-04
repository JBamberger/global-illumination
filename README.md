
# Global Illumination Methods: PathTracer WS1920

This repository contains a pathtracer written in C++. The tracer works on a static scene and only uses the CPU. The tracing process is parallelized with OpenMP. Some features of the application include:

- Bounding-box-based hit-tests
- An Octree for the scene
- Bounding volume hierarchies (BVH) for the models in the scene
- Lambertian, metal-like and dielectric material
- Basic texturing support
- A simple obj reader

## Results

### Number of samples

The following images are sampled 8, 32, 64, 128, 256, 512, 1024, 2048, 4096 and 8192 times respectively.

![Cornell box 8 samples][cornell-8]
![Cornell box 32 samples][cornell-32]
![Cornell box 64 samples][cornell-64]
![Cornell box 128 samples][cornell-128]
![Cornell box 256 samples][cornell-256]
![Cornell box 512 samples][cornell-512]
![Cornell box 1024 samples][cornell-1024]
![Cornell box 2048 samples][cornell-2048]
![Cornell box 4096 samples][cornell-4096]
![Cornell box 8192 samples][cornell-8192]

### Texture mapping

Simple texture mapping capabilities are available:

![Spot the cow in a Cornell Box, 2048 samples][spot]

### Rendering performance

The following tests were performed on a machine with Intel Core i7-6700 (4 cores 8 threads @3.4 Ghz base clock). The sample count is 2048 for all of the following images:

![Stanford Dragon (Diffuse), 2048 samples][dragon-lambertian]
![Stanford Dragon (Glass), 2048 samples][dragon-dielectric]

The model of the Stanford dragon used here consists of roughly 47800 triangles. Rendering the diffuse/lambertian version took 550 seconds. The glass version took 755 seconds.

![Pig, 2048 samples][pig]

In comparison, this model of a pig consists of roughly 11400 triangles and was rendered in 430 seconds.

| Name              | Primitives    | Rendertime    |
| ----------------- | ------------- | ------------- |
| Cornell box scene | ~30           | 210           |
| Cow (diffuse)     | 5856          | 285           |
| Cow (texture)     | 5856          | 295           |
| Pig               | 11400         | 430           |
| Dragon (diffuse)  | 47800         | 550           |
| Dragon (glass)    | 47800         | 755           |


## Dependencies and prerequisites

- [``cmake``][cmake]
- [``qt5``][qt]
- ``glm``
- ``gtest``

### vcpkg

[Vcpkg][vcpkg] is a cross-platform package manager for C and C++ libraries.

```bash
vcpkg install qt5
vcpkg install glm
vcpkg install gtest
```

### Windows
On Windows you can install the open-source version of the Qt library using the binaries provided by [www.qt.io][qt]. CMake can also be installed via binary packages from [www.cmake.org][cmake]. It is recommended to install the dependencies into the proposed directories; CMake will then find the libraries automatically.

### Linux
Linux users can use their favorite package manager to install the dependencies:

* Ubuntu: ``sudo apt-get install qt5 cmake``
* Arch: ``sudo pacman -S qt5 cmake``
* Fedora: ``sudo yum install qt5 cmake``

### macOS
The easiest way to install the dependencies for macOS is to first install [Homebrew][brew]. You can then install the dependencies by typing ``brew install qt5 cmake`` into the command line.:w

### Building

Most modern IDEs come with CMake support (e.g. CLion, Visual Studio 2019, VS Code). For a build from the command line use the following commands:

~~~Bash
  cd <project-root>
  mkdir build; cd build
  cmake ..
  cmake --build . --config Release
  ./app/global-illu <project-root>/share
~~~

For dependencies installed with vcpkg add ``-DCMAKE_TOOLCHAIN_FILE="<vcpkg-root>/scripts/buildsystems/vcpkg.cmake"`` to the ``cmake ..`` command.

On Windows you can use the graphical UI of CMake to first configure your project and then generate project files for your IDE (for example Visual Studio).

[qt]: https://www.qt.io/download-open-source/
[cmake]: https://cmake.org/download/
[brew]: www.brew.sh
[vcpkg]: https://github.com/Microsoft/vcpkg

[cornell-8]: ./results/cornell-8.png "Cornell box 8 samples"
[cornell-32]: ./results/cornell-32.png "Cornell box 32 samples"
[cornell-64]: ./results/cornell-64.png "Cornell box 64 samples"
[cornell-128]: ./results/cornell-128.png "Cornell box 128 samples"
[cornell-256]: ./results/cornell-256.png "Cornell box 256 samples"
[cornell-512]: ./results/cornell-512.png "Cornell box 512 samples"
[cornell-1024]: ./results/cornell-1024.png "Cornell box 1024 samples"
[cornell-2048]: ./results/cornell-2048.png "Cornell box 2048 samples"
[cornell-4096]: ./results/cornell-4096.png "Cornell box 4096 samples"
[cornell-8192]: ./results/cornell-8192.png "Cornell box 8192 samples"
[spot]: ./results/spot-2048.png "Spot in Cornell box, 2048 samples"
[dragon-dielectric]: ./results/dragon-2048.png "Stanford Dragon (Glass) in Cornell box, 2048 samples"
[dragon-lambertian]: ./results/dragon-2048-lambertian.png "Stanford Dragon (Diffuse) in Cornell box, 2048 samples"
[pig]: ./results/pig-2048.png "Pig in Cornell box, 2048 samples"