#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// function that shifts the alphabet array to left
// first letter gets to be the last
void shiftAlphabetToLeft(char *alphabet) {
    int alphabetSize = strlen(alphabet);
    char temp = alphabet[0];

    for (int i = 1; i < alphabetSize; i ++) {
        alphabet[i - 1] = alphabet[i];
    }

    alphabet[strlen(alphabet) - 1] = temp;
}

// populates the shift matrix needed in order to
// decrypt the Vigenere cipher
// square matrix with the size of the alphabet
void populateShiftMatrix(char *alphabet, char **shiftMatrix) {
    int matrixSize = strlen(alphabet);
    char *alphabetBackup = (char*)malloc(strlen(alphabet) + 1);

    strcpy(alphabetBackup, alphabet);

    for (int i = 0; i < matrixSize; i ++) {

        for (int j = 0; j < matrixSize; j ++) {
            shiftMatrix[i][j] = alphabetBackup[j];
        }

        shiftMatrix[i][matrixSize] = '\0';

        shiftAlphabetToLeft(alphabetBackup);
    }
}

// print the shift matrix
// used for testing the function above
void printShiftMatrix(char **shiftMatrix) {
    int matrixSize = strlen(shiftMatrix[0]);

    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            printf("%c ", shiftMatrix[i][j]);
        }
        printf("\n");
    }
}

// repeats the key until the resulted table has the
// same length with the cyphered text
void fillKeyTable(char *key, int size, char *keyTable) {
    int keySize = strlen(key);
    int keyRepeat = size / keySize;
    int remaining = size - (keyRepeat * keySize);
    int iterator = 0;

    for (int i = 0; i < keyRepeat; i++) {
        for (int j = 0; j < keySize; j++) {
            keyTable[iterator] = key[j];
            iterator++;
        }
    }

    for (int k = 0; k < remaining; k++) {
        keyTable[iterator] = key[k];
        iterator++;
    }

    keyTable[iterator] = '\0';
}

// function that applies the algorithm explained
// during the laboratory
void decodeVigenereCipher(char *cipheredText, char *decipheredText,
                          char *alphabet, char* key) {

    int alphabetSize = strlen(alphabet);
    int cipheredSize = strlen(cipheredText);
    char **shiftMatrix = (char **)malloc(cipheredSize * sizeof(char *));

    for (int i = 0; i < cipheredSize; i++) {
        shiftMatrix[i] = (char *)malloc(cipheredSize * sizeof(char) + 1);
    }

    populateShiftMatrix(alphabet, shiftMatrix);

    char *keyTable = (char*)malloc(cipheredSize * sizeof(char) + 1);
    fillKeyTable(key, cipheredSize, keyTable);

    // for every ciphered letter
    for (int k = 0; k < cipheredSize; k++) {
        // search the line
        for (int i = 0; i < alphabetSize; i++) {
            if (keyTable[k] == shiftMatrix[i][0]) {
                // search the column
                for (int j = 0; j < alphabetSize; j ++) {
                    if (cipheredText[k] == shiftMatrix[i][j]) {
                        decipheredText[k] = shiftMatrix[0][j];
                    }
                }
            }
        }
    }

    decipheredText[cipheredSize] = '\0';
}

int main() {

    char *alphabet = (char*)"abcdefghijklmnopqrstuvwxyz";

    // no spacing required, only lowercase alphabet letters
    char *cipheredText = (char*)"ciffrlxmtgpvkchsimfgsiewtyex";
    char *key = (char*)"lemon";

    int cipheredSize = strlen(cipheredText);
    char *decipheredText = (char*)malloc(cipheredSize * sizeof(char) + 1);

    decodeVigenereCipher(cipheredText, decipheredText, alphabet, key);

    printf("The ciphered text is: %s\n", cipheredText);
    printf("The key is: %s\n", key);
    printf("The deciphered text is: %s\n", decipheredText);

    return 0;
}