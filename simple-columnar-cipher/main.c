// SIMPLE COLUMNAR CIPHER IMPLEMENTED BY ALEXANDRU BUSE-DRAGOMIR
// CEN4S1A 29.11.2019 / 30.11.2019

// will use the method of encoding and decoding that assumes text
// to be displayed in rows, not columns
// for the second version, the single difference is in the
// way in which result is read
// the process of ordering and moving columns is the same

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#define MAX_KEY_SIZE 1000

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

const char *key = "lemonade";
const char* plainText = "thefrencharmyhassufferedmanycasualties";

void orderStringLexicographically(const char *string, char *orderedString) {
    strcpy(orderedString, string);

    for (unsigned int i = 0; i < strlen(orderedString) - 1; i ++) {
        for (unsigned int j = i + 1; j < strlen(orderedString); j ++) {
            if (strcmp(orderedString + i, orderedString + j) > 0) {
                char aux = orderedString[i];
                orderedString[i] = orderedString[j];
                orderedString[j] = aux;
            }
        }
    }
}

// used for testing
void printCipherTable(char **cipherTable) {
    int columnNumber = strlen(key);
    // get rows as double
    // round if there is one not complete row
    // not considering the incomplete last row
    int completeRowNumber = (int)(ceil(strlen(plainText) / (columnNumber * 1.0)));

    printf("\n");

    for (int i = 0; i < completeRowNumber + 1; i ++) {
        for (int j = 0; j < columnNumber; j ++) {
            printf("%c ", cipherTable[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}

void createCipherTable(const char *key, const char *plainText, char **cipherTable) {
    int columnNumber = strlen(key);
    // number of complete rows
    // + 1 as the first row is reserved for the key
    int completeRowNumber = strlen(plainText) / strlen(key) + 1;
    // entries on last, incomplete row
    int sparseEntries = strlen(plainText) - columnNumber * (completeRowNumber - 1);

    // first add the key
    for (int j = 0; j < columnNumber; j ++) {
        cipherTable[0][j] = key[j];
    }

    // then add the plain text
    for (int i = 1; i < completeRowNumber; i ++) {
        for (int j = 0; j < columnNumber; j ++) {
            cipherTable[i][j] = plainText[(i - 1) * columnNumber + j];
        }
    }

    // continue to fill the last row
    for (int k = 0; k < columnNumber; k ++) {
        // number of letters already added
        int filledCounter = (completeRowNumber - 1) * columnNumber;

        if (k < sparseEntries) {
            cipherTable[completeRowNumber][k] = plainText[filledCounter + k];
        } else {
            cipherTable[completeRowNumber][k] = '-';
        }
    }
}

void createDecipherTable(const char *key, const char *cipheredText, char **decipherTable) {
    int numberOfColumns = strlen(key);
    int numberOfCompleteRows = strlen(cipheredText) / strlen(key);
    int numberOfSparseEntries = strlen(cipheredText) - numberOfCompleteRows * numberOfColumns;

    // we fill the complete rows first
    // we add an additional line for the key
    // that is ordered
    char *orderedKey = malloc(strlen(key) * sizeof(char) + 1);
    orderStringLexicographically(key, orderedKey);

    // complete the header row with the ordered key
    for (int j = 0; j < numberOfColumns; j ++) {
        decipherTable[0][j] = orderedKey[j];
    }

    // now we fill the complete rows
    for (int i = 1; i < numberOfCompleteRows + 1; i ++) {
        for (int j = 0; j < numberOfColumns; j ++) {
            decipherTable[i][j] = cipheredText[(i - 1) * numberOfColumns + j];
        }
    }

    // create an array of found letters in order
    // to avoid picking the same ordered letter
    // for a key letter that appears many times
    int *alreadyFound = malloc(numberOfColumns * sizeof(int));
    for (int k = 0; k < numberOfColumns; k ++) {
        alreadyFound[k] = 0;
    }

    // we search all the positions from the last
    // where a letter must be added
    for (int i = 0; i < numberOfSparseEntries; i ++) {
        char keyLetter = key[i];

        for (int j = 0; j < numberOfColumns; j ++) {
            // we found a column that must have a letter
            if (orderedKey[j] == keyLetter && alreadyFound[j] == 0) {
                alreadyFound[j] = 1;
                // do not search further
                break;
            }
        }
    }

    // in the end, knowing the positions
    // that must have a letter, we parse
    // trough the ciphered text and add them
    // in the other places, we add a dummy sign, like '-'
    int counter = 0;

    for (int j = 0; j < numberOfColumns; j ++) {
        if (alreadyFound[j] == 0) {
            decipherTable[numberOfCompleteRows + 1][j] = '-';
        } else {
            decipherTable[numberOfCompleteRows + 1][j] =
                    cipheredText[numberOfColumns * numberOfCompleteRows + counter];
            counter ++;
        }
    }
}

void copyCipherTableColumn(char **fromTable, char **toTable, int rowNumber,
        int fromColumnNumber, int toColumnNumber) {

    for (int i = 0; i < rowNumber; i ++) {
        toTable[i][toColumnNumber] = fromTable[i][fromColumnNumber];
    }
}

void getOrderedCipherTable(const char *key, char **cipherTable,
        char **orderedCipherTable, int rowNumber) {

    int numberOfColumns = strlen(key);

    char *orderedKey = (char *)malloc(numberOfColumns * sizeof(char *));
    orderStringLexicographically(key, orderedKey);

    // this will be used for the case of non unique letters
    int positionFound[MAX_KEY_SIZE] = {0};

    // for each letter of the key
    for (int i = 0; i < numberOfColumns; i ++) {
        // we will move this column
        int fromColumnNumber = i;
        int toColumnNumber;

        // we search the right position of the column
        for (int j = 0; j < numberOfColumns; j ++) {
            // if we find a match in sorted and the match
            // has not been found before
            if (key[i] == orderedKey[j] && positionFound[j] == 0) {
                toColumnNumber = j;
                positionFound[j] = 1;

                // put the column in order
                copyCipherTableColumn(cipherTable, orderedCipherTable, rowNumber,
                        fromColumnNumber, toColumnNumber);

                // do not check for a match any further
                break;
            }
        }
    }
}

void getUnorderedCipherTable(const char *key, char **decipherTable,
        char **unorderedCipherTable, int rowNumber) {

    int numberOfColumns = strlen(key);

    // this will be used for the case of non unique letters
    int positionFound[MAX_KEY_SIZE] = {0};

    char *orderedKey = malloc(strlen(key) * sizeof(char) + 1);
    orderStringLexicographically(key, orderedKey);

    // for each letter of the ordered key
    for (int i = 0; i < numberOfColumns; i ++) {
        char orderedKeyLetter = orderedKey[i];
        int fromColumnNumber = i;
        int toColumnNumber;

        // we find the occurrence of that letter
        // inside the key
        for (int j = 0; j < numberOfColumns; j ++) {
            // if we find a match in the unsorted and the match
            // has not been found before
            if (orderedKeyLetter == key[j] && positionFound[j] == 0) {
                toColumnNumber = j;
                positionFound[j] = 1;

                // put the column in initial order
                copyCipherTableColumn(decipherTable, unorderedCipherTable, rowNumber,
                                      fromColumnNumber, toColumnNumber);

                // do not check for a match any further
                break;
            }
        }
    }
}

void encodeSimpleColumnarCipher(const char *key, const char *plainText, char *cipheredText) {
    // number of rows as double
    double rowNumberDouble = strlen(plainText) / (strlen(key) * 1.0);
    // round if there is not complete row
    // add 1 for the key row
    int rowNumber = (int)(ceil(rowNumberDouble)) + 1;
    // number of columns is equal with the key length
    int columnNumber = strlen(key);

    char **cipherTable = (char **)malloc(rowNumber * sizeof(char *));
    for (int i = 0; i < rowNumber; i ++) {
        cipherTable[i] = (char *)malloc(columnNumber * sizeof(char) + 1);
    }

    char **orderedCipherTable = (char **)malloc(rowNumber * sizeof(char *));
    for (int i = 0; i < rowNumber; i ++) {
        orderedCipherTable[i] = (char *)malloc(columnNumber * sizeof(char) + 1);
    }

    createCipherTable(key, plainText, cipherTable);
    getOrderedCipherTable(key, cipherTable, orderedCipherTable, rowNumber);

    // number of complete rows
    // + 1 as the first row is reserved for the key
    int completeRowNumber = strlen(plainText) / strlen(key) + 1;

    int counter = 0;

    // we also take the last, incomplete row
    for (int i = 1; i < completeRowNumber + 1; i ++) {
        for (int j = 0; j < columnNumber; j ++) {
            if (orderedCipherTable[i][j] != '-') {
                cipheredText[counter] = orderedCipherTable[i][j];
                counter ++;
            }
        }
    }

    cipheredText[counter] = '\0';
}

void decodeSimpleColumnarCipher(const char *key, const char *cipheredText, char *decipheredText) {
    // number of rows as double
    double rowNumberDouble = strlen(plainText) / (strlen(key) * 1.0);
    // round if there is not complete row
    // add 1 for the key row
    int rowNumber = (int)(ceil(rowNumberDouble)) + 1;
    // number of columns is equal with the key length
    int columnNumber = strlen(key);

    char **decipherTable = (char **)malloc(rowNumber * sizeof(char *));
    for (int i = 0; i < rowNumber; i ++) {
        decipherTable[i] = (char *)malloc(columnNumber * sizeof(char) + 1);
    }

    char **unorderedCipherTable = (char **)malloc(rowNumber * sizeof(char *));
    for (int i = 0; i < rowNumber; i ++) {
        unorderedCipherTable[i] = (char *)malloc(columnNumber * sizeof(char) + 1);
    }

    createDecipherTable(key, cipheredText, decipherTable);
    getUnorderedCipherTable(key, decipherTable, unorderedCipherTable, rowNumber);

    // number of complete rows
    // + 1 as the first row is reserved for the key
    int completeRowNumber = strlen(plainText) / strlen(key) + 1;

    int counter = 0;

    // we also take the last, incomplete row
    for (int i = 1; i < completeRowNumber + 1; i ++) {
        for (int j = 0; j < columnNumber; j ++) {
            if (unorderedCipherTable[i][j] != '-') {
                decipheredText[counter] = unorderedCipherTable[i][j];
                counter ++;
            }
        }
    }

    decipheredText[counter] = '\0';
}

int main() {

    char *cipheredText = (char *)malloc(strlen(plainText) * sizeof(char *));
    char *decipheredText = (char *)malloc(strlen(plainText) * sizeof(char *));

    printf("Plain text: %s\n", plainText);

    encodeSimpleColumnarCipher(key, plainText, cipheredText);
    printf("Ciphered text: %s\n", cipheredText);

    decodeSimpleColumnarCipher(key, cipheredText, decipheredText);
    printf("Deciphered text: %s\n", decipheredText);

    assert(strcmp(plainText, decipheredText) == 0);

    return 0;
}