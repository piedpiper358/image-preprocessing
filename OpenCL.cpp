#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#include "CL/opencl.h"
#include <CL/cl.hpp>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#include "OpenCL.hpp" 
#include "image.h"

//#include <string.h> 
//#include <time.h>
//#include <exception>
//#define __CL_ENABLE_EXCEPTIONS
#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)
#define num_of_kernels (4)

extern const char* invitation;
extern const char* invalid_input;
extern const char* delimiter;

cl_command_queue command_queue=NULL;
cl_context context=NULL;
cl_program program=NULL;
cl_int ret = CL_SUCCESS;



void  checkRet(cl_int ret)
{
	if (ret)
	{
		printf("FATAL ERROR!! RET:  %i\n", ret);
		//throw std::invalid_argument("setPos illegal argument!");
		exit(ret);
	}
}
void opencl(image_t const * const source, image_t* dest){
	cl_uint ret_num_platforms;
	cl_uint ret_num_devices;
	
	cl_int sel_num = -1;
	cl_device_type device_type;

	char string[MEM_SIZE];
	bool isRight = false;
	//------------------------------------------------------------------------
	FILE *fp;
	char fileName[] = "./src/Kernels.cl";
	char *source_str;
	size_t source_size;

	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels.\n");
		exit(EXIT_FAILURE);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	//-------------------------------------------------------------------------

	checkRet(clGetPlatformIDs(0, NULL, &ret_num_platforms));
		
	printf("HeloooOOOoooOOO\n");
	cl_platform_id *platform_id = new cl_platform_id[ret_num_platforms];
	checkRet(clGetPlatformIDs(ret_num_platforms, platform_id, &ret_num_platforms));


	while (!isRight)
	{
		printf("\nFounded %i platform(s). Select one of the following platforms:\n", ret_num_platforms);
		for (int i = 0; i < ret_num_platforms; i++)
		{
			checkRet(clGetPlatformInfo(platform_id[i], CL_PLATFORM_NAME, sizeof(string), &string, NULL));
			printf("\n%i - %s ", i, string);
			checkRet(clGetPlatformInfo(platform_id[i], CL_PLATFORM_VENDOR, sizeof(string), &string, NULL));
			printf("by %s\n", string);
		}
		printf("\n%s", invitation);
		scanf("%i", &sel_num);
		isRight = sel_num >= 0 && sel_num < ret_num_platforms;
		if (!isRight)
		{
			printf("%s", invalid_input);
			rewind(stdin);
		}
	};
	checkRet(clGetDeviceIDs(platform_id[sel_num], CL_DEVICE_TYPE_ALL, 0, NULL, &ret_num_devices));
	cl_device_id *device_id = new cl_device_id[ret_num_devices];
	checkRet(clGetDeviceIDs(platform_id[sel_num], CL_DEVICE_TYPE_ALL, ret_num_devices, device_id, &ret_num_devices));
	checkRet(clGetPlatformInfo(platform_id[sel_num], CL_PLATFORM_NAME, sizeof(string), &string, NULL));

	isRight = false;

	cl_uint dimensions = 0;
	while (!isRight)
	{
		printf("\nThe list of devices available on %s platform:\n", string);
		for (int i = 0; i < ret_num_devices; i++)
		{
			checkRet(clGetDeviceInfo(device_id[i], CL_DEVICE_NAME, sizeof(string), &string, NULL));
			printf("\n%i - %s ", i, string);
			checkRet(clGetDeviceInfo(device_id[i], CL_DEVICE_VERSION, sizeof(string), &string, NULL));
			printf("which supports %s ", string);
			checkRet(clGetDeviceInfo(device_id[i], CL_DEVICE_VENDOR, sizeof(string), &string, NULL));
			printf("by %s ", string);
			checkRet(clGetDeviceInfo(device_id[i], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL));
			
			switch (device_type)
			{
			case CL_DEVICE_TYPE_CPU:
				printf("(CPU)");
				break;
			case CL_DEVICE_TYPE_GPU:
				printf("(GPU)");
				break;
			case CL_DEVICE_TYPE_ACCELERATOR:
				printf("(Accelerator)");
				break;
			case CL_DEVICE_TYPE_DEFAULT:
				printf("(Default)");
				break;
			case CL_DEVICE_TYPE_CUSTOM:
				printf("(Custom)");
				break;
			}
			checkRet(clGetDeviceInfo(device_id[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 
									 sizeof(dimensions), &dimensions, NULL));
			cl_uint *NDRange = new cl_uint[dimensions];
			checkRet(clGetDeviceInfo(device_id[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, 
									 malloc_usable_size(NDRange), NDRange, NULL));

			printf(" \nNDRange: (");
			for (int i = 0; i < dimensions; i++)
			{
				printf("%u", NDRange[i]);
				if (i < dimensions - 1)
					printf(", ");
			}
			printf(")\n");
		}

		printf("\n%s", invitation);
		scanf("%i", &sel_num);
		isRight = sel_num >= 0 && sel_num < ret_num_devices;
		if (!isRight)
		{
			printf("%s", invalid_input);
			rewind(stdin);
		}
		
	};
	//----------------------------------------------------------------------------------------------------------------------------
	checkRet(clGetDeviceInfo(device_id[sel_num], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(dimensions), &dimensions, NULL));
	cl_uint *NDRange = new cl_uint[dimensions];
	checkRet(clGetDeviceInfo(device_id[sel_num], CL_DEVICE_MAX_WORK_ITEM_SIZES, malloc_usable_size(NDRange), NDRange, NULL));

	isRight = false;
	cl_uint row1 = 0, row2 = 0, col1 = 0, col2 = 0;
	

	
	context = clCreateContext(NULL, 1, &device_id[sel_num], NULL, NULL, &ret);
	checkRet(ret);
	/*cl_queue_properties props[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};*/
	

	command_queue = clCreateCommandQueue/*WithProperties*/(context, device_id[sel_num], CL_QUEUE_PROFILING_ENABLE 
																			/*| CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/  /*0*/, &ret);
	checkRet(ret);

	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	checkRet(ret);
	
	char* options = (char*)"-cl-std=CL2.0  "; /*-I ../lab6/include*/
	ret = clBuildProgram(program, 1, &device_id[sel_num], options, NULL, NULL);
	//-------------------------------------------------------------------------------------------------------------------------
	if (ret == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device_id[sel_num], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *) malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, device_id[sel_num], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		
		// Print the log
		printf("%s\n", log);
	}
	//-------------------------------------------------------------------------------------------------------------------------

	/*kernels[2] = clCreateKernel(program, "erode", &ret);
	checkRet(ret);*/
	//size_t global_work_size0[2] = { source->height, source->width };
}


void clgreyscale(image_t const * const source, image_t* dest){
	size_t global_work_size0[2] = { source->height, source->width };
		cl_kernel kernel = clCreateKernel(program, "greyscale", &ret);
		checkRet(ret);
	
		cl_mem arg0 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(source->data), source->data, &ret);
		checkRet(ret);
		cl_mem arg1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(dest->data), dest->data, &ret);
		checkRet(ret);

		checkRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arg0));
		checkRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&arg1));
		checkRet(clSetKernelArg(kernel, 2, sizeof(source->width), &source->width));

		checkRet(clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size0, NULL, 0, NULL,NULL));

		/*if(source==dest){free(source->data);}*/
	
		checkRet(clEnqueueReadBuffer(command_queue, arg1, CL_TRUE, 0, malloc_usable_size(dest->data), dest->data, 0, NULL, NULL));
		//D =(float*) clEnqueueMapBuffer(command_queue, arg2,CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, malloc_usable_size(D) ,0, NULL, NULL, &ret);

	}

void clbinarization(image_t const * const source, image_t* dest, int threshold ){
	size_t global_work_size0[2] = { source->height, source->width };
	cl_kernel kernel = clCreateKernel(program, "binarization", &ret);
	checkRet(ret);
	
	cl_mem arg0 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(source->data), source->data, &ret);
	checkRet(ret);
	cl_mem arg1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(dest->data), dest->data, &ret);
	checkRet(ret);

	checkRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arg0));
	checkRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&arg1));
	checkRet(clSetKernelArg(kernel, 2, sizeof(source->width), &source->width));
	checkRet(clSetKernelArg(kernel, 3, sizeof(threshold), &threshold));

	checkRet(clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size0, NULL, 0, NULL,NULL));

	/*if(source==dest){free(source->data);}*/
	
	checkRet(clEnqueueReadBuffer(command_queue, arg1, CL_TRUE, 0, malloc_usable_size(dest->data), dest->data, 0, NULL, NULL));

}

void cldilate(image_t const * const source, image_t* dest){
	
	size_t global_work_size0[2] = { source->height, source->width };
	cl_kernel kernel = clCreateKernel(program, "dilate", &ret);
	checkRet(ret);
	
	cl_mem arg0 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(source->data), source->data, &ret);
	checkRet(ret);
	cl_mem arg1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(dest->data), dest->data, &ret);
	checkRet(ret);

	checkRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arg0));
	checkRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&arg1));
	checkRet(clSetKernelArg(kernel, 2, sizeof(source->width), &source->width));


	checkRet(clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size0, NULL, 0, NULL,NULL));

	/*if(source==dest){free(source->data);}*/
	checkRet(clEnqueueReadBuffer(command_queue, arg1, CL_TRUE, 0, malloc_usable_size(dest->data), dest->data, 0, NULL, NULL));

}	

void clerode(image_t const * const source, image_t* dest){
	size_t global_work_size0[2] = { source->height, source->width };
	cl_kernel kernel = clCreateKernel(program, "erode", &ret);
	checkRet(ret);
	
	cl_mem arg0 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(source->data), source->data, &ret);
	checkRet(ret);
	cl_mem arg1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(dest->data), dest->data, &ret);
	checkRet(ret);

	checkRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arg0));
	checkRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&arg1));
	checkRet(clSetKernelArg(kernel, 2, sizeof(source->width), &source->width));


	checkRet(clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size0, NULL, 0, NULL,NULL));

	/*if(source==dest){free(source->data);}*/
	checkRet(clEnqueueReadBuffer(command_queue, arg1, CL_TRUE, 0, malloc_usable_size(dest->data), dest->data, 0, NULL, NULL));

}	

	
void clopening(image_t const * const source, image_t* dest){
	
	clerode(source, dest);
	cldilate(dest, dest);
}
void clclosing(image_t const * const source, image_t* dest){

	cldilate(source, dest);
	clerode(dest, dest);
}
void colours2bin(image_t const * const source, image_t* dest, int threshold ){
	
	clgreyscale(source, dest);
	clbinarization(source, dest, threshold);
}

void tophat(image_t const * const source, image_t* dest){
	//doesn't working
	clopening(source, dest);
	clsubtraction(source, dest, dest);
}


void clsubtraction(image_t const * const minuend, image_t const * const subtrahend, image_t* dest){
		size_t global_work_size0[2] = { minuend->height, minuend->width };
		cl_kernel kernel = clCreateKernel(program, "subtraction", &ret);
		checkRet(ret);
	
		cl_mem arg0 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(minuend->data), minuend->data, &ret);
		checkRet(ret);
		cl_mem arg1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(subtrahend->data), subtrahend->data, &ret);
		checkRet(ret);
		cl_mem arg2 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(dest->data), dest->data, &ret);
		checkRet(ret);

		checkRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arg0));
		checkRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&arg1));
		checkRet(clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&arg2));
		checkRet(clSetKernelArg(kernel, 3, sizeof(minuend->width), &minuend->width));

		checkRet(clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size0, NULL, 0, NULL,NULL));

		/*if(source==dest){free(source->data);}*/
	
		checkRet(clEnqueueReadBuffer(command_queue, arg2, CL_TRUE, 0, malloc_usable_size(dest->data), dest->data, 0, NULL, NULL));

	}


void clboarder(image_t const * const source, image_t* dest /*, clboardertype type*/, char* type, int threshold  ){
	size_t global_work_size0[2] = { source->height, source->width };
	cl_kernel kernel = clCreateKernel(program, type/*"roberts"*/, &ret);
	if(ret)
	{printf("Wrong parameter");}
	
	//checkRet(ret);
	
	cl_mem arg0 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(source->data), source->data, &ret);
	checkRet(ret);
	cl_mem arg1 = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, malloc_usable_size(dest->data), dest->data, &ret);
	checkRet(ret);

	checkRet(clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&arg0));
	checkRet(clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&arg1));
	checkRet(clSetKernelArg(kernel, 2, sizeof(source->width), &source->width));
	checkRet(clSetKernelArg(kernel, 3, sizeof(threshold), &threshold ));


	checkRet(clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size0, NULL, 0, NULL,NULL));

	/*if(source==dest){free(source->data);}*/
	checkRet(clEnqueueReadBuffer(command_queue, arg1, CL_TRUE, 0, malloc_usable_size(dest->data), dest->data, 0, NULL, NULL));

}
