
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#include <CL/cl.hpp>
#endif








//#include <iostream.h>
const int g_cuNumItems = 128;
const char *g_pcszSource =
"__kernel void memset(__global uint puDst) \n"
"{ \n"
" puDst[get_global_id(0)] = get_global_id(0); \n"
"} \n";

int main()
{
	//
	// 1. ��������� ��������
	//
	cl_uint uNumPlatforms;
	clGetPlatformIDs(0, NULL, &uNumPlatforms);
	std::cout << uNumPlatforms << " platforms" << std::endl;
	cl_platform_id *pPlatforms = new cl_platform_id[uNumPlatforms];
	clGetPlatformIDs(uNumPlatforms, pPlatforms, &uNumPlatforms);




	//
	// 2. ��������� ������ ����������
	//
	cl_device_id deviceID;
	cl_uint uNumGPU;
	clGetDeviceIDs(
		/*pPlatforms[0], CL_DEVICE_TYPE_GPU, 1, &deviceID, &uNumGPU);*/

	pPlatforms[0], CL_DEVICE_TYPE_CPU, 1, &deviceID, &uNumGPU);
	//
	// 3. �������� ���������
	//
	cl_context context = clCreateContext(
		NULL, 1, &deviceID, NULL, NULL, NULL);

	//
	// 4. �������� ������� ������
	//
	cl_command_queue queue = clCreateCommandQueue(
		context, deviceID, 0, NULL);
	//
	// 5. �������� ���������
	//
	cl_program program = clCreateProgramWithSource(
		context, 1, &g_pcszSource, NULL, NULL);



	//
	// 6. ������ ���������
	//
	cl_int errcode = clBuildProgram(
		program, 1, &deviceID, NULL, NULL, NULL);
	//
	// 7. ��������� ����
	//
	cl_kernel kernel = clCreateKernel(program, "memset", NULL);




	//
	// 8. �������� ������
	//
	cl_mem buffer = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY,
		g_cuNumItems * sizeof(cl_uint), NULL, NULL);
	//
	// 9. ��������� ������ � �������� ��������� ����
	//
	clSetKernelArg(kernel, 0, sizeof(buffer), (void *)&buffer);





	//
	// 10. ������ ����
	//
	size_t uGlobalWorkSize = g_cuNumItems;
	clEnqueueNDRangeKernel(
		queue, kernel, 1, NULL, &uGlobalWorkSize,
		NULL, 0, NULL, NULL);
	clFinish(queue);




	//
	// 11. ����������� ������ � ������ ������������ ����
	//
	cl_uint *puData = (cl_uint *)clEnqueueMapBuffer(
		queue, buffer, CL_TRUE, CL_MAP_READ, 0,
		g_cuNumItems * sizeof(cl_uint), 0, NULL, NULL, NULL);


	//
	// 12. ������������� �����������
	//
	for (int i = 0; i < g_cuNumItems; ++i)
		std::cout << i << " - " << puData[i] << "; ";
	std::cout << std::endl;
	//
	// 13. ���������� ����������� ������
	//
	clEnqueueUnmapMemObject(
		queue, buffer, puData, 0, NULL, NULL);


	//
	// 14. �������� �������� � ������������ ������
	// ������������ ����
	//
	clReleaseMemObject(buffer);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	delete[] pPlatforms;
	//
} // main()