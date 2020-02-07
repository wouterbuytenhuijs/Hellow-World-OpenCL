#include <stdio.h>
#include <iostream>
#include <CL/cl.h>
#include <CL/cl.hpp>
#include <fstream>

int main() {

	//Find available devices
	std::vector<cl::Device> devices;
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices_available;
	int n = 0; // number of available devices
	cl::Platform::get(&platforms);
	for (int i = 0; i < (int)platforms.size(); i++) {
		devices_available.clear();
		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices_available);
		if (devices_available.size() == 0) continue; // no device found in plattform i
		for (int j = 0; j < (int)devices_available.size(); j++) {
			n++;
			devices.push_back(devices_available[j]);
		}
	}
	if (platforms.size() == 0 || devices.size() == 0) {
		std::cout << "Error: There are no OpenCL devices available!" << std::endl;
		return -1;
	}
	for (int i = 0; i < n; i++) std::cout << "ID: " << i << ", Device: " << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;

	_ASSERT(devices.size() > 0);

	auto device = devices[1]; //select correct device

	// Execute Kernel
	std::ifstream helloWorldFile("Template.cl");
	std::string src(std::istreambuf_iterator<char>(helloWorldFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));
	cl::Context context(device);
	cl::Program program(context, sources);

	auto err = program.build("-cl-std=CL1.2");

	char buf[16];
	cl::Buffer memBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(buf));
	cl::Kernel kernel(program, "HelloWorld", &err);

	kernel.setArg(0, memBuf); // Set parameters

	cl::CommandQueue queue(context, device);
	queue.enqueueTask(kernel);
	queue.enqueueReadBuffer(memBuf, CL_TRUE, 0, sizeof(buf), buf);

	std::cout << buf;
	std::cin.get();
	return 0;
}