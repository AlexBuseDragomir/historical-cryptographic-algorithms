// AUTOKEY CIPHER IMPLEMENTED BY ALEXANDRU BUSE-DRAGOMIR
// CEN4S1A 27.11.2019

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void appendLetterToString(char *string, char letter) {
    int stringSize = strlen(string);

    string[stringSize] = letter;
    string[stringSize + 1] = '\0';
}

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

// same length with the cyphered text
// key and plain text repeated until key table is full
void fillKeyTable(char *key, int size, char *keyTable, const char *plainText) {
    int keySize = strlen(key);

    // add the key
    strcpy(keyTable, key);

    for (int i = keySize; i < size; i ++) {
        keyTable[i] = plainText[i - keySize];
    }
}

void encodeAutokeyCipher(char *plainText, char *cipheredText,
                         char *alphabet, char *key) {

    int alphabetSize = strlen(alphabet);
    int plainTextSize = strlen(plainText);

    // populate the shift matrix
    char **shiftMatrix = (char **)malloc(plainTextSize * sizeof(char *));

    for (int i = 0; i < plainTextSize; i++) {
        shiftMatrix[i] = (char *)malloc(plainTextSize * sizeof(char) + 1);
    }

    populateShiftMatrix(alphabet, shiftMatrix);

    // populate the key table
    char *keyTable = (char*)malloc(plainTextSize * sizeof(char) + 1);
    fillKeyTable(key, plainTextSize, keyTable, plainText);

    // for every plain text letter
    for (int k = 0; k < plainTextSize; k++) {
        // search the column
        for (int j = 0; j < alphabetSize; j++) {
            if (plainText[k] == shiftMatrix[0][j]) {
                // search the line
                for (int i = 0; i < alphabetSize; i ++) {
                    if (keyTable[k] == shiftMatrix[i][0]) {
                        // found the cipher letter
                        cipheredText[k] = shiftMatrix[i][j];
                    }
                }
            }
        }
    }

    cipheredText[plainTextSize] = '\0';
}

void decodeAutokeyCipher(char *cipheredText, char *decipheredText,
                          char *alphabet, char* key) {

    int alphabetSize = strlen(alphabet);
    int cipheredSize = strlen(cipheredText);
    char **shiftMatrix = (char **)malloc(cipheredSize * sizeof(char *));

    for (int i = 0; i < cipheredSize; i++) {
        shiftMatrix[i] = (char *)malloc(cipheredSize * sizeof(char) + 1);
    }

    populateShiftMatrix(alphabet, shiftMatrix);

    char *keyTable = (char*)malloc(cipheredSize * sizeof(char) + 1);

    // add the key at first
    // then add the deciphered as it is processed
    strcpy(keyTable, key);

    // for every ciphered letter
    for (int k = 0; k < cipheredSize; k++) {
        // search the line
        for (int i = 0; i < alphabetSize; i++) {
            if (keyTable[k] == shiftMatrix[i][0]) {
                // search the column
                for (int j = 0; j < alphabetSize; j ++) {

                    if (cipheredText[k] == shiftMatrix[i][j]) {
                        decipheredText[k] = shiftMatrix[0][j];

                        // append the plain letter at the end of the key
                        if (strlen(keyTable) < (unsigned int)cipheredSize) {
                            appendLetterToString(keyTable, decipheredText[k]);
                        }
                    }
                }
            }
        }
    }
    printf("\nKey table is: %s\n", keyTable);
    decipheredText[cipheredSize] = '\0';
}

int main() {

    char *alphabet = (char*)"abcdefghijklmnopqrstuvwxyz";

    // no spacing required, only lowercase alphabet letters
    char *plainText = "theamericanarmyispreparingtoattack";
    char *key = (char*)"lemonade";

    int plainTextSize = strlen(plainText);

    char *cipheredText = (char*)malloc(plainTextSize * sizeof(char) + 1);
    char *decipheredText = (char*)malloc(plainTextSize * sizeof(char) + 1);

    encodeAutokeyCipher(plainText, cipheredText, alphabet, key);
    decodeAutokeyCipher(cipheredText, decipheredText, alphabet, key);

    printf("Plain text: %s\n", plainText);
    printf("Ciphered text: %s\n", cipheredText);
    printf("Deciphered text: %s\n", decipheredText);

    assert(strcmp(plainText, decipheredText) == 0);

    return 0;
}