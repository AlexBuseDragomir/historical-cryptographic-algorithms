// AUTHOR: BUSE-DRAGOMIR ALEXANDRU, CEN4S1A
// DATE 06.12.2019

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// we use a default period
// in this case, the same from
// the laboratory
const int period = 5;
const int alphabetSize = 26;
const int tableSize = 5;

char *key = "lemonjuice";
char *plainText = "thearmyismovingforwardtowardsthecapital";

struct position {
    int row;
    int column;
};

void initializeArrayWithValue(int *array, int arraySize, int value) {
    for (int i = 0; i < arraySize; i ++) {
        array[i] = value;
    }
}

void lowercaseText(char *text, char *result) {
    for (unsigned int i = 0; i < strlen(text); i ++) {
        result[i] = (char)tolower(text[i]);
    }

    result[strlen(text)] = '\0';
}

void printKeyTable(char **keyTable, int tableSize) {
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            printf("%c ", keyTable[i][j]);
        }
        printf("\n");
    }
}

void appendArrays(const int *first, const int *second, int size, int *result) {
    for (int i = 0; i < size; i ++) {
        result[i] = first[i];
    }
    for (int i = size; i < 2 * size; i ++) {
        result[i] = second[i - size];
    }
}

void splitArrayInTwo(const int *input, int *first, int *second, int initialSize) {
    for (int i = 0; i < initialSize / 2; i ++) {
        first[i] = input[i];
    }
    for (int i = initialSize / 2; i < initialSize; i ++) {
        second[i - initialSize / 2] = input[i];
    }
}

void appendCharArrays(char *first, char *second, char *result) {
    strcpy(result, first);
    strcat(result, second);
}

struct position getPositionForLetter(char letter, char **keyTable) {
    struct position p;
    p.row = -1;
    p.column = -1;

    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            if (keyTable[i][j] == 'i' && letter == 'j') {
                p.row = i;
                p.column = j;
            }
            if (keyTable[i][j] == letter) {
                p.row = i;
                p.column = j;
            }
        }
    }

    return p;
}

// removes duplicates and also
// considers i and j as one letter
void removeDuplicatesFromKey(char *key, char *result, char *remainingLetters) {
    // we will use a appearance array
    int *letterAppearances = malloc(alphabetSize * sizeof(int) + 1);

    // initialize the letter appearances to 0
    initializeArrayWithValue(letterAppearances, alphabetSize, 0);

    int counter = 0;
    char currentLetter;

    for (unsigned int i = 0; i < strlen(key); i ++) {
        currentLetter = key[i];

        // particular case for i and j
        if ((currentLetter == 'i' || currentLetter == 'j')
            && letterAppearances['i' - 'a'] == 0) {

            result[counter] = 'i';
            letterAppearances['i' - 'a'] = 1;
            letterAppearances['j' - 'a'] = 1;
            counter ++;
        }

        // letter has not been yet found
        if (letterAppearances[currentLetter - 'a'] == 0) {
            result[counter] = currentLetter;
            letterAppearances[currentLetter - 'a'] = 1;
            counter ++;
        }
    }

    result[counter] = '\0';

    // now get the remaining letters

    counter = 0;

    for (int k = 0; k < alphabetSize; k ++) {
        // we must omit j from remaining letters
        if (letterAppearances[k] == 0 && ('a' + k) != 'j') {
            remainingLetters[counter] = (char)('a' + k);
            counter ++;
        }
    }

    remainingLetters[counter] = '\0';
}

void populateKeyTable(char **keyTable, char *key) {

    char *result = malloc(strlen(key) * sizeof(char) + 1);
    char *remainingLetters = malloc(tableSize * tableSize * sizeof(char) + 1);
    char *keyTableContent = malloc(tableSize * tableSize * sizeof(char) + 1);

    // lowercase the key
    lowercaseText(key, result);
    strcpy(key, result);

    // remove duplicates
    removeDuplicatesFromKey(key, result, remainingLetters);
    strcpy(key, result);

    // we concatenate the two arrays
    appendCharArrays(result, remainingLetters, keyTableContent);

    // we populate the key table
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            keyTable[i][j] = keyTableContent[i * tableSize + j];
        }
    }
}

void encodeBifidCipher(char *key, char *plainText, char *cipheredText) {

    char **keyTable = (char **)malloc(tableSize * sizeof(char *));

    for (int i = 0; i < tableSize; i ++) {
        keyTable[i] = (char *)malloc(tableSize * sizeof(char) + 1);
    }

    populateKeyTable(keyTable, key);

    int completePeriods = strlen(plainText) / period;
    int remainingLetters = strlen(plainText) % period;

    int counter = 0;

    // first for complete pairs
    for (int i = 0; i < completePeriods; i ++) {
        // we use two arrays for encoding
        int *lineCoordinates = malloc(period * sizeof(int));
        int *columnCoordinates = malloc(period * sizeof(int));

        int startIndex = i * period;
        int endIndex = (i + 1) * period;

        for (int j = startIndex; j < endIndex; j ++) {
            struct position letterPosition =
                    getPositionForLetter(plainText[j], keyTable);

            lineCoordinates[j - startIndex] = letterPosition.row + 1;
            columnCoordinates[j - startIndex] = letterPosition.column + 1;
        }

        int *result = malloc(2 * period * sizeof(int));
        appendArrays(lineCoordinates, columnCoordinates, period, result);

        for (int k = 0; k < period * 2; k += 2) {
            int row = result[k];
            int column = result[k + 1];

            cipheredText[counter] = keyTable[row - 1][column - 1];
            counter ++;
        }
    }

    // if there are remaining letters
    // do the same for them
    if (remainingLetters != 0) {
        // we use two arrays for encoding
        int *lineCoordinates = malloc(remainingLetters * sizeof(int));
        int *columnCoordinates = malloc(remainingLetters * sizeof(int));

        for (int i = completePeriods * period; i < strlen(plainText); i ++) {
            struct position letterPosition =
                    getPositionForLetter(plainText[i], keyTable);

            lineCoordinates[i - completePeriods * period] = letterPosition.row + 1;
            columnCoordinates[i - completePeriods * period] = letterPosition.column + 1;
        }

        int *result = malloc(2 * remainingLetters * sizeof(int));
        appendArrays(lineCoordinates, columnCoordinates, remainingLetters, result);

        for (int k = 0; k < remainingLetters * 2; k += 2) {
            int row = result[k];
            int column = result[k + 1];

            cipheredText[counter] = keyTable[row - 1][column - 1];
            counter ++;
        }
    }
    cipheredText[counter] = '\0';
}

void decodeBifidCipher(char *key, char *cipheredText, char *decipheredText) {
    char **keyTable = (char **)malloc(tableSize * sizeof(char *));

    for (int i = 0; i < tableSize; i ++) {
        keyTable[i] = (char *)malloc(tableSize * sizeof(char) + 1);
    }

    populateKeyTable(keyTable, key);

    int completePeriods = strlen(plainText) / period;
    int remainingLetters = strlen(plainText) % period;

    int counter = 0;

    // first for complete pairs
    for (int i = 0; i < completePeriods; i ++) {
        int *bothRows = malloc(2 * period * sizeof(int));

        int startIndex = i * period;
        int endIndex = (i + 1) * period;

        for (int j = startIndex; j < endIndex; j ++) {
            struct position letterPosition =
                    getPositionForLetter(cipheredText[j], keyTable);

            bothRows[(j - startIndex) * 2] = letterPosition.row + 1;
            bothRows[(j - startIndex) * 2 + 1] = letterPosition.column + 1;
        }

        int *lineCoordinates = malloc(remainingLetters * sizeof(int));
        int *columnCoordinates = malloc(remainingLetters * sizeof(int));

        splitArrayInTwo(bothRows, lineCoordinates, columnCoordinates, 2 * period);

        for (int k = 0; k < period; k ++) {
            int row = lineCoordinates[k] - 1;
            int column = columnCoordinates[k] - 1;

            decipheredText[counter] = keyTable[row][column];
            counter ++;
        }
    }

    // then for the remaining letters
    // if there are any
    if (remainingLetters != 0) {
        int *bothRows = malloc(2 * remainingLetters * sizeof(int));

        for (int i = completePeriods * period; i < strlen(cipheredText); i ++) {
            struct position letterPosition =
                    getPositionForLetter(cipheredText[i], keyTable);

            bothRows[(i - completePeriods * period) * 2] = letterPosition.row + 1;
            bothRows[(i - completePeriods * period) * 2 + 1] = letterPosition.column + 1;
        }

        int *lineCoordinates = malloc(remainingLetters * sizeof(int));
        int *columnCoordinates = malloc(remainingLetters * sizeof(int));

        splitArrayInTwo(bothRows, lineCoordinates, columnCoordinates, 2 * remainingLetters);

        for (int k = 0; k < remainingLetters; k ++) {
            int row = lineCoordinates[k] - 1;
            int column = columnCoordinates[k] - 1;

            decipheredText[counter] = keyTable[row][column];
            counter ++;
        }
    }
    decipheredText[counter] = '\0';
}

int main() {

    char *cipheredText = (char *)malloc(strlen(plainText) + 1);
    char *decipheredText = (char *)malloc(strlen(plainText) + 1);
    char **keyTable = (char **)malloc(tableSize * sizeof(char *));

    for (int i = 0; i < tableSize; i ++) {
        keyTable[i] = (char *)malloc(tableSize * sizeof(char) + 1);
    }

    populateKeyTable(keyTable, key);
    encodeBifidCipher(key, plainText, cipheredText);
    decodeBifidCipher(key, cipheredText, decipheredText);

    printf("\nPolybius square for key\n\n");
    printKeyTable(keyTable, tableSize);
    printf("\nPlain text: %s\n", plainText);
    printf("Ciphered text: %s\n", cipheredText);
    printf("Deciphered text: %s\n", decipheredText);

    assert(strcmp(plainText, decipheredText) == 0);

    return 0;
}