
# Global Illumination Methods: Path tracing

This repository contains a path tracer written in C++. The tracer works on a static scene and only uses the CPU. The tracing process is parallelized with OpenMP. Some features of the application include:

- Bounding-box-based hit-tests
- An Octree for the scene
- Bounding volume hierarchies (BVH) for the models in the scene
- Lambertian, metal-like and dielectric material
- Basic texturing support
- A simple obj reader

## Results

![Exam scene][exam-scene]

### Number of samples

The following images are sampled 8, 32, 64, 128, 256, 512, 1024, 2048, 4096 and 8192 times respectively.

#### Cornell box 8 samples

![Cornell box 8 samples][cornell-8]

#### Cornell box 32 samples

![Cornell box 32 samples][cornell-32]

#### Cornell box 64 samples

![Cornell box 64 samples][cornell-64]

#### Cornell box 128 samples

![Cornell box 128 samples][cornell-128]

#### Cornell box 256 samples

![Cornell box 256 samples][cornell-256]

#### Cornell box 512 samples

![Cornell box 512 samples][cornell-512]

#### Cornell box 1024 samples

![Cornell box 1024 samples][cornell-1024]

#### Cornell box 2048 samples

![Cornell box 2048 samples][cornell-2048]

#### Cornell box 4096 samples

![Cornell box 4096 samples][cornell-4096]

#### Cornell box 8192 samples

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

[`cmake`][cmake], [`qt5`][qt], [`glm`][glm], [`gtest`][gtest]

### vcpkg
The easiest way to build the project is to install the dependencies with vcpkg. [Vcpkg][vcpkg] is a cross-platform package manager for C and C++ libraries.

```bash
vcpkg install qt5
vcpkg install glm
vcpkg install gtest
```

### Windows

CMake can also be installed from [cmake.org][cmake]. The libraries can be downloaded and installed separately. To make them available to CMake create environment variables with `<libname>_ROOT` and the path to the library as value.

### Linux

Cmake and qt can be installed with most package managers. The other libraries must be downloaded separately.

- Ubuntu: `sudo apt-get install qt5 cmake`
- Arch: `sudo pacman -S qt5 cmake`
- Fedora: `sudo yum install qt5 cmake`

### macOS

If [Homebrew][brew] is available install Qt and CMake with it `brew install qt5 cmake`. Otherwise install the dependencies manually.

## Building

Most modern IDEs come with CMake support (e.g. CLion, Visual Studio 2019, VS Code) or have plugins to enable such support. Then it is sufficient to open the project folder and build one of the targets.

For a build from the command line use the following commands:

```Bash
  cd <project-root>
  mkdir build; cd build
  cmake ..
  cmake --build . --config Release
  ./app/global-illu <project-root>/share
```

For dependencies installed with vcpkg add `-DCMAKE_TOOLCHAIN_FILE="<vcpkg-root>/scripts/buildsystems/vcpkg.cmake"` to the `cmake ..` command.

On Windows you can use the graphical UI of CMake to first configure your project and then generate project files for your IDE (for example Visual Studio).

[qt]: https://www.qt.io/download-open-source/
[glm]: https://github.com/g-truc/glm
[gtest]: https://github.com/google/googletest
[cmake]: https://cmake.org/download/
[brew]: www.brew.sh
[vcpkg]: https://github.com/Microsoft/vcpkg

[exam-scene]: ./results/exam-scene.jpg "Exam scene"
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
