#pragma once

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <iostream>

typedef struct CLInfo {
	cl_context context;
	cl_command_queue command_queue;
	cl_device_id* device_list;
} CLInfo;

static CLInfo* createCLContext() {
	//Platform creation
	cl_platform_id* platforms = NULL;
	cl_uint num_platforms;

	cl_int clStatus = clGetPlatformIDs(0, NULL, &num_platforms);
	platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id)*num_platforms);

	clStatus = clGetPlatformIDs(num_platforms, platforms, NULL);

	//Device creation
	cl_device_id* device_list = NULL;
	cl_uint num_devices;

	for (int i = 0; i < num_platforms; i++) {
		clStatus = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU,
			0, NULL, &num_devices);

		device_list = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);

		clStatus = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU,
			num_devices, device_list, NULL);

		if (clStatus == 0) {
			goto found;
		}
	}

	free(device_list);

	std::cout << "Could not find GPU!\n";
	std::cout << "Looking for CPU device...\n";

	for (int i = 0; i < num_platforms; i++) {
		clStatus = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_CPU,
			0, NULL, &num_devices);

		device_list = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);

		clStatus = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_CPU,
			num_devices, device_list, NULL);

		if (clStatus == 0) {
			goto found;
		}
	}
	
	free(device_list);

	std::cout << "Could not find CPU!\n";
	std::cout << "Not OpenCL context created!\n";
	return NULL;

found:
	//Context creation
	cl_context context;
	context = clCreateContext(NULL, num_devices,
		device_list, NULL, NULL, &clStatus);

	cl_command_queue command_queue = clCreateCommandQueue(
		context, device_list[0], 0, &clStatus);

	CLInfo* clInfo = (CLInfo*)malloc(sizeof(CLInfo));
	clInfo->command_queue = command_queue;
	clInfo->context = context;
	clInfo->device_list = device_list;

	return clInfo;
}