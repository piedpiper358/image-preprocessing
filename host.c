
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
	// 1. Получение платформ
	//
	cl_uint uNumPlatforms;
	clGetPlatformIDs(0, NULL, &uNumPlatforms);
	std::cout << uNumPlatforms << " platforms" << std::endl;
	cl_platform_id *pPlatforms = new cl_platform_id[uNumPlatforms];
	clGetPlatformIDs(uNumPlatforms, pPlatforms, &uNumPlatforms);




	//
	// 2. Получение номера видеокарты
	//
	cl_device_id deviceID;
	cl_uint uNumGPU;
	clGetDeviceIDs(
		/*pPlatforms[0], CL_DEVICE_TYPE_GPU, 1, &deviceID, &uNumGPU);*/

	pPlatforms[0], CL_DEVICE_TYPE_CPU, 1, &deviceID, &uNumGPU);
	//
	// 3. Создание контекста
	//
	cl_context context = clCreateContext(
		NULL, 1, &deviceID, NULL, NULL, NULL);

	//
	// 4. Создание очереди команд
	//
	cl_command_queue queue = clCreateCommandQueue(
		context, deviceID, 0, NULL);
	//
	// 5. Создание программы
	//
	cl_program program = clCreateProgramWithSource(
		context, 1, &g_pcszSource, NULL, NULL);



	//
	// 6. Сборка программы
	//
	cl_int errcode = clBuildProgram(
		program, 1, &deviceID, NULL, NULL, NULL);
	//
	// 7. Получение ядра
	//
	cl_kernel kernel = clCreateKernel(program, "memset", NULL);




	//
	// 8. Создание буфера
	//
	cl_mem buffer = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY,
		g_cuNumItems * sizeof(cl_uint), NULL, NULL);
	//
	// 9. Установка буфера в качестве аргумента ядра
	//
	clSetKernelArg(kernel, 0, sizeof(buffer), (void *)&buffer);





	//
	// 10. Запуск ядра
	//
	size_t uGlobalWorkSize = g_cuNumItems;
	clEnqueueNDRangeKernel(
		queue, kernel, 1, NULL, &uGlobalWorkSize,
		NULL, 0, NULL, NULL);
	clFinish(queue);




	//
	// 11. Отображение буфера в память управляющего узла
	//
	cl_uint *puData = (cl_uint *)clEnqueueMapBuffer(
		queue, buffer, CL_TRUE, CL_MAP_READ, 0,
		g_cuNumItems * sizeof(cl_uint), 0, NULL, NULL, NULL);


	//
	// 12. Использование результатов
	//
	for (int i = 0; i < g_cuNumItems; ++i)
		std::cout << i << " - " << puData[i] << "; ";
	std::cout << std::endl;
	//
	// 13. Завершение отображения буфера
	//
	clEnqueueUnmapMemObject(
		queue, buffer, puData, 0, NULL, NULL);


	//
	// 14. Удаление объектов и освобождение памяти
	// управляющего узла
	//
	clReleaseMemObject(buffer);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	delete[] pPlatforms;
	//
} // main()