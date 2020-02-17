#include "rt.h"

#include <CL/cl.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

cl_float4* cpu_out;
cl::CommandQueue queue;
cl::Kernel kernel;
cl::Context context;
cl::Program program;
cl::Buffer cl_out;

void init()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::cout << "Available platforms:" << std::endl;
    for (const auto& p : platforms) {
        std::cout << "  " << p.getInfo<CL_PLATFORM_NAME>() << std::endl;
    }
    if (platforms.empty()) {
        std::terminate(); // TODO: handle
    }

    cl::Platform pf = platforms[0];
    std::vector<cl::Device> devices;
    pf.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    std::cout << "Available devices:" << std::endl;
    for (const auto& dev : devices) {
        std::cout << dev.getInfo<CL_DEVICE_NAME>() << "("
                  << dev.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << ","
                  << dev.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << ")" << std::endl;
    }
    if (devices.empty()) {
        std::terminate();
    }

    cl::Device device = devices[0];

    context = cl::Context(device);
    queue = cl::CommandQueue(context, device);

    std::ifstream kernel_source_stream("kernel.cl");
    std::stringstream kernel_source_buffer;
    kernel_source_buffer << kernel_source_stream.rdbuf();

    program = cl::Program(context, kernel_source_buffer.str().c_str());

    const auto compile_result = program.build({device});
    if (compile_result) {
        std::cerr << "Compilation error in compute kernel code." << std::endl;
        if (compile_result == CL_BUILD_PROGRAM_FAILURE) {
            std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
            std::cerr << "Build log:\n" << log << std::endl;
        }
        std::terminate();
    }

    kernel = cl::Kernel(program, "renderer");
}

bool test()
{
    const auto w = 500;
    const auto h = 500;

    cpu_out = new cl_float3[w * h];

    init();

    cl_out = cl::Buffer(context, CL_MEM_WRITE_ONLY, w * h * sizeof(cl_float3));

    kernel.setArg(0, cl_out);
    kernel.setArg(1, w);
    kernel.setArg(2, h);

    const size_t work_size = w * h;
    const size_t local_size = 64;

    queue.enqueueNDRangeKernel(kernel, NULL, work_size, local_size);
    const auto result = queue.finish();
    if (result != CL_SUCCESS) {
        if (result == CL_OUT_OF_HOST_MEMORY) {
            std::cerr << "Out of host memory." << std::endl;
        } else {
            std::cerr << "Failed to execute command." << std::endl;
        }
        std::terminate();
    }
    queue.enqueueReadBuffer(cl_out, CL_TRUE, 0, w * h * sizeof(cl_float3), cpu_out);

    delete[] cpu_out;
}