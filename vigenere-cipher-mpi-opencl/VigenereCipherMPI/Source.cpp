#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <math.h>

// english alphabet size
constexpr auto ALPHABET_SIZE = 26;
// number of logical processors
constexpr auto PROCESSOR_NUMBER = 4;
// size of data to be encrypted
constexpr auto DATA_SIZE = 16;
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
void computeCipheredText(char *plainText, char *key, int size, char cipherTable[ALPHABET_SIZE][ALPHABET_SIZE]);
void printArray(char *array, int size);

int main(int argc, char **argv)
{
	// process identificator
	int pid;
	// number of processes used
	int numberOfProcesses;
	// array of initial data / to be ciphered
	char dataArray[DATA_SIZE];
	// text under the form of a string
	std::string text = "retreatbeforesix";
	// the key used in order to encrypt the data
	char keyArray[DATA_SIZE];
	// key under the form of a string
	std::string key = "flower";
	// chunk of data per each process (has size / 4)
	char processArray[DATA_SIZE / PROCESSOR_NUMBER];
	// chunk of key data per each process (has size / 4)
	char processKeyArray[DATA_SIZE / PROCESSOR_NUMBER];
	// final result combines results from each process
	char finalArray[DATA_SIZE];
	// the table containing the cipher
	char cipherTable[ALPHABET_SIZE][ALPHABET_SIZE];

	// initialising the dataArray with the text to be encrypted
	fillCharArrayFromString(dataArray, text, DATA_SIZE);
	// initialising the key array with the characters 
	// the size must be equal with that of the dataArray
	// so a special method is used in order to repeat the
	// key until the sizes are equal
	fillKeyArrayFromString(keyArray, key, DATA_SIZE);
	// initialise the cipherTable with the corresponding values
	populateCipherTable(cipherTable);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	if (pid == 0) {
		std::cout << "The initial text is: " << text << std::endl;
		std::cout << "The key is: " << key << std::endl;
	}

	// send chunks of data to each process
	MPI_Scatter(dataArray, DATA_SIZE / numberOfProcesses, MPI_CHAR, processArray,
		DATA_SIZE / numberOfProcesses, MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Scatter(keyArray, DATA_SIZE / numberOfProcesses, MPI_CHAR, processKeyArray,
		DATA_SIZE / numberOfProcesses, MPI_CHAR, 0, MPI_COMM_WORLD);

	// each process performs the encryption
	computeCipheredText(processArray, processKeyArray, DATA_SIZE / numberOfProcesses, cipherTable);

	// gather all data into a single array in the end
	MPI_Gather(processArray, DATA_SIZE / numberOfProcesses, MPI_CHAR,
		finalArray, DATA_SIZE / numberOfProcesses, MPI_CHAR, 0, MPI_COMM_WORLD);

	// from root process, print the resulting code
	if (pid == 0) {
		std::cout << "The encrypted text is: ";
		printArray(finalArray, DATA_SIZE);
		std::cout << std::endl;
	}

	MPI_Finalize();
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

// ciphered text will be saved in the plainText pointer
// letters will be encrypted one by one
void computeCipheredText(char *plainText, char *key, int size, char cipherTable[ALPHABET_SIZE][ALPHABET_SIZE]) {
	for (int i = 0; i < size; i++) {
		char plainLetter = plainText[i];
		char keyLetter = key[i];
		// this is the column
		int plainIndex = (int)(plainLetter - 'a');
		// this is the row
		int keyIndex = (int)(keyLetter - 'a');
		char resultedChar = cipherTable[keyIndex][plainIndex];
		plainText[i] = resultedChar;
	}
}

void printArray(char *array, int size) {
	for (int i = 0; i < size; i++) {
		printf("%c ", array[i]);
	}
}