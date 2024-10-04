__kernel void vector_add(__global const char *initial_array, __global const char *key_array, 
	__global char *result_array) {

	// Get the index of the current element to be processed
	int index = get_global_id(0);

	int size = 16;
	int alphabet_size = 26;

	// the english alphabet
	char ALPHABET_ARRAY[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
										'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

	char line[26];
	char cipherTable[26][26];

	for (int i = 0; i < alphabet_size; i++) {
		line[i] = ALPHABET_ARRAY[i];
	}

	for (int i = 0; i < alphabet_size; i++) {
		for (int j = 0; j < alphabet_size; j++) {
			cipherTable[i][j] = line[j];
		}

		int temp = line[0];

		for (int i = 0; i < alphabet_size - 1; i++) {
			line[i] = line[i + 1];
		}
		line[alphabet_size - 1] = temp;
	}

	char initial_letter = initial_array[index];
	char key_letter = key_array[index];
	// this is the column
	int initial_index = (int)(initial_letter - 'a');
	// this is the row
	int key_index = (int)(key_letter - 'a');
	char resulted_letter = cipherTable[key_index][initial_index];
	result_array[index] = resulted_letter;
}