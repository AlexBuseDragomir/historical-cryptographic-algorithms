//@Alexandru Buse-Dragomir, CEN 3.1A

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <math.h>

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4996)

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)


// english alphabet size
constexpr auto ALPHABET_SIZE = 26;
// size of data to be encrypted
constexpr auto ARRAY_SIZE = 16;
// the english alphabet
constexpr char ALPHABET_ARRAY[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
									'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

void populateArrayWithValues(char *array, char *values, int size);
void populateArrayWithConstantValues(char *array, char *values, int size);
void shiftArrayLeft(char* arrayToShift, int size);
void populateCipherTable(char cipherArray[ALPHABET_SIZE][ALPHABET_SIZE]);
void printCipherTable(char array[ALPHABET_SIZE][ALPHABET_SIZE]);
void fillCharArrayFromString(char *array, std::string text, int size);
void fillKeyArrayFromString(char *result, std::string key, int dataSize);
void printArray(char *array, int size);
void computeCipheredText(char *plainText, char *key, int size,
	char cipherTable[ALPHABET_SIZE][ALPHABET_SIZE], char* resultText);

int main(void) {
	printf("started running\n");

	// text under the form of a string
	std::string text = "retreatbeforesix";

	// key under the form of a string
	std::string key = "flower";

	char *initial_array = (char*)malloc(sizeof(char)*ARRAY_SIZE);
	char *key_array = (char*)malloc(sizeof(char)*ARRAY_SIZE);
	char *result_array = (char*)malloc(sizeof(char)*ARRAY_SIZE);

	// initialising the dataArray with the text to be encrypted
	fillCharArrayFromString(initial_array, text, ARRAY_SIZE);

	// initialising the key array with the characters 
	// the size must be equal with that of the dataArray
	// so a special method is used in order to repeat the
	// key until the sizes are equal
	fillKeyArrayFromString(key_array, key, ARRAY_SIZE);

	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("vector_cypher_kernel.cl", "r");

	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	
	printf("kernel loading done\n");
	
	// Get platform and device information
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;


	cl_int ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
	cl_platform_id *platforms = NULL;
	platforms = (cl_platform_id*)malloc(ret_num_platforms * sizeof(cl_platform_id));

	ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);
	printf("ret at %d is %d\n", __LINE__, ret);

	ret = clGetDeviceIDs(platforms[1], CL_DEVICE_TYPE_ALL, 1,
		&device_id, &ret_num_devices);
	printf("ret at %d is %d\n", __LINE__, ret);

	// Create an OpenCL context
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	printf("ret at %d is %d\n", __LINE__, ret);

	// Create a command queue
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	printf("ret at %d is %d\n", __LINE__, ret);

	// Create memory buffers on the device for each vector 
	cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		ARRAY_SIZE * sizeof(char), NULL, &ret);
	cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,
		ARRAY_SIZE * sizeof(char), NULL, &ret);
	cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
		ARRAY_SIZE * sizeof(char), NULL, &ret);

	// Copy the lists A and B to their respective memory buffers
	ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0,
		ARRAY_SIZE * sizeof(char), initial_array, 0, NULL, NULL);
	printf("ret at %d is %d\n", __LINE__, ret);

	ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0,
		ARRAY_SIZE * sizeof(char), key_array, 0, NULL, NULL);
	printf("ret at %d is %d\n", __LINE__, ret);

	printf("before building\n");
	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1,
		(const char **)&source_str, (const size_t *)&source_size, &ret);
	printf("ret at %d is %d\n", __LINE__, ret);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	printf("ret at %d is %d\n", __LINE__, ret);

	printf("after building\n");
	// Create the OpenCL kernel
	cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);
	printf("ret at %d is %d\n", __LINE__, ret);

	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
	printf("ret at %d is %d\n", __LINE__, ret);

	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
	printf("ret at %d is %d\n", __LINE__, ret);

	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);
	printf("ret at %d is %d\n", __LINE__, ret);

	//added this to fix garbage output problem
	//ret = clSetKernelArg(kernel, 3, sizeof(int), &LIST_SIZE);

	printf("before execution\n");

	// Execute the OpenCL kernel on the list
	size_t global_item_size = ARRAY_SIZE; // Process the entire lists
	size_t local_item_size = 16; // Divide work items into groups of 16

	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL,
		&global_item_size, &local_item_size, 0, NULL, NULL);

	printf("after execution\n");

	// Read the memory buffer result_array on
	// the device to the local variable result_array
	ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0,
		ARRAY_SIZE * sizeof(char), result_array, 0, NULL, NULL);

	printf("after copying\n");

	// Display the result to the screen
	std::cout << "\n\n";

	std::cout << "The initial text is: \n";
	printArray(initial_array, ARRAY_SIZE);
	std::cout << "\n";

	std::cout << "The key is: \n";
	std::cout << key;
	std::cout << "\n";

	std::cout << "The encrypted text is: \n";
	printArray(result_array, ARRAY_SIZE);
	std::cout << std::endl;

	// Clean up
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(a_mem_obj);
	ret = clReleaseMemObject(b_mem_obj);
	ret = clReleaseMemObject(c_mem_obj);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	free(initial_array);
	free(key_array);
	free(result_array);
	
	return 0;
}

// methods 
void populateArrayWithValues(char *array, char *values, int size) {
	for (int i = 0; i < size; i++) {
		array[i] = values[i];
	}
}

void populateArrayWithConstantValues(char *array, const char *values, int size) {
	for (int i = 0; i < size; i++) {
		array[i] = values[i];
	}
}

void shiftArrayLeft(char* arrayToShift, int size) {
	int temp = arrayToShift[0];
	for (int i = 0; i < size - 1; i++) {
		arrayToShift[i] = arrayToShift[i + 1];
	}
	arrayToShift[size - 1] = temp;
}

void populateCipherTable(char cipherArray[ALPHABET_SIZE][ALPHABET_SIZE]) {
	char line[ALPHABET_SIZE];
	populateArrayWithConstantValues(line, ALPHABET_ARRAY, ALPHABET_SIZE);
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		populateArrayWithValues(cipherArray[i], line, ALPHABET_SIZE);
		shiftArrayLeft(line, ALPHABET_SIZE);
	}
}

void printCipherTable(char array[ALPHABET_SIZE][ALPHABET_SIZE]) {
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		for (int j = 0; j < ALPHABET_SIZE; j++) {
			printf("%c ", array[i][j]);
		}
		printf("\n");
	}
}

void fillCharArrayFromString(char *array, std::string text, int size) {
	for (int i = 0; i < size; i++) {
		array[i] = text[i];
	}
}

void fillKeyArrayFromString(char *result, std::string key, int dataSize) {
	int keySize = key.size();
	int times = dataSize / keySize;
	int spaceLeft = dataSize - (times * keySize);
	int counter = 0;
	for (int i = 0; i < times; i++) {
		for (int j = 0; j < keySize; j++) {
			result[counter] = key[j];
			counter++;
		}
	}
	for (int i = 0; i < spaceLeft; i++) {
		result[counter] = key[i];
		counter++;
	}
}

void printArray(char *array, int size) {
	for (int i = 0; i < size; i++) {
		printf("%c ", array[i]);
	}
}

// ciphered text will be saved in the plainText pointer
// letters will be encrypted one by one
void computeCipheredText(char *plainText, char *key, int size,
	char cipherTable[ALPHABET_SIZE][ALPHABET_SIZE], char* resultText) {

	for (int i = 0; i < size; i++) {
		char plainLetter = plainText[i];
		char keyLetter = key[i];
		// this is the column
		int plainIndex = (int)(plainLetter - 'a');
		// this is the row
		int keyIndex = (int)(keyLetter - 'a');
		char resultedChar = cipherTable[keyIndex][plainIndex];
		resultText[i] = resultedChar;
	}
}