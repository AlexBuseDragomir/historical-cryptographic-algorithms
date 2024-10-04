// AUTHOR: BUSE-DRAGOMIR ALEXANDRU, CEN4S1A

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

const char* alphabet = "abcdefghijklmnopqrstuvwxyz";
const int alphabetSize = 26;
const int tableSize = 5;

char *key = "anotherrandomkey";
char *plainText = "thearmyismovingforwardtowardsberlin";

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
            if (keyTable[i][j] == i && letter == 'j') {
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

void cypherTextUsingPlayFair(char *plainText, char *ciphered, char **keyTable) {
    // if text has uneven number of characters
    // add an 'x'
    char *modifiedPlainText = malloc(strlen(plainText) + 2);
    strcpy(modifiedPlainText, plainText);

    if (strlen(plainText) % 2 != 0) {
        modifiedPlainText[strlen(plainText)] = 'x';
        modifiedPlainText[strlen(plainText) + 1] = '\0';
    }

    for (unsigned int i = 0; i < strlen(modifiedPlainText); i += 2) {
        // if a letter repeats in a pair
        // we exchange it with a 'x'
        if (modifiedPlainText[i] == modifiedPlainText[i + 1]) {
            modifiedPlainText[i + 1] = 'x';
        }
    }

    // the cipher process
    for (unsigned int i = 0; i < strlen(modifiedPlainText); i += 2) {
        struct position firstLetterPosition =
                getPositionForLetter(modifiedPlainText[i], keyTable);
        struct position secondLetterPosition =
                getPositionForLetter(modifiedPlainText[i + 1], keyTable);

        int differenceModRow = abs(firstLetterPosition.row - secondLetterPosition.row);
        int differenceModColumn = abs(firstLetterPosition.column - secondLetterPosition.column);

        // case 1, same row
        if (differenceModRow == 0) {
            int row = firstLetterPosition.row;
            int column1 = firstLetterPosition.column;
            int column2 = secondLetterPosition.column;

            if (firstLetterPosition.column == tableSize - 1) {
                ciphered[i] = keyTable[row][0];
            } else {
                ciphered[i] = keyTable[row][column1 + 1];
            }

            if (secondLetterPosition.column == tableSize - 1) {
                ciphered[i + 1] = keyTable[row][0];
            } else {
                ciphered[i + 1] = keyTable[row][column2 + 1];
            }
        }

        // case 2, same column
        if (differenceModColumn == 0) {
            int row1 = firstLetterPosition.row;
            int row2 = secondLetterPosition.row;
            int column = firstLetterPosition.column;

            if (firstLetterPosition.row == tableSize - 1) {
                ciphered[i] = keyTable[0][column];
            } else {
                ciphered[i] = keyTable[row1 + 1][column];
            }

            if (secondLetterPosition.row == tableSize - 1) {
                ciphered[i + 1] = keyTable[0][column];
            } else {
                ciphered[i + 1] = keyTable[row2 + 1][column];
            }
        }

        // case 3, different row, different column
        if (differenceModRow != 0 && differenceModColumn != 0) {
            int row1 = firstLetterPosition.row;
            int row2 = secondLetterPosition.row;
            int column1 = firstLetterPosition.column;
            int column2 = secondLetterPosition.column;

            ciphered[i] = keyTable[row1][column2];
            ciphered[i + 1] = keyTable[row2][column1];
        }
    }

    ciphered[strlen(modifiedPlainText)] = '\0';
}

// quite similar with the encrypt
// just reversed
void decypherTextUsingPlayFair(char *cipheredText, char *decipheredText, char **keyTable) {

    // the decipher process
    for (unsigned int i = 0; i < strlen(cipheredText); i += 2) {
        struct position firstLetterPosition =
                getPositionForLetter(cipheredText[i], keyTable);
        struct position secondLetterPosition =
                getPositionForLetter(cipheredText[i + 1], keyTable);

        int differenceModRow = abs(firstLetterPosition.row - secondLetterPosition.row);
        int differenceModColumn = abs(firstLetterPosition.column - secondLetterPosition.column);

        // case 1, same row
        if (differenceModRow == 0) {
            int row = firstLetterPosition.row;
            int column1 = firstLetterPosition.column;
            int column2 = secondLetterPosition.column;

            if (firstLetterPosition.column == 0) {
                decipheredText[i] = keyTable[row][tableSize - 1];
            } else {
                decipheredText[i] = keyTable[row][column1 - 1];
            }

            if (secondLetterPosition.column == 0) {
                decipheredText[i + 1] = keyTable[row][tableSize - 1];
            } else {
                decipheredText[i + 1] = keyTable[row][column2 - 1];
            }
        }

        // case 2, same column
        if (differenceModColumn == 0) {
            int row1 = firstLetterPosition.row;
            int row2 = secondLetterPosition.row;
            int column = firstLetterPosition.column;

            if (firstLetterPosition.row == 0) {
                decipheredText[i] = keyTable[tableSize - 1][column];
            } else {
                decipheredText[i] = keyTable[row1 - 1][column];
            }

            if (secondLetterPosition.row == 0) {
                decipheredText[i + 1] = keyTable[tableSize - 1][column];
            } else {
                decipheredText[i + 1] = keyTable[row2 - 1][column];
            }
        }

        // case 3, different row, different column
        if (differenceModRow != 0 && differenceModColumn != 0) {
            int row1 = firstLetterPosition.row;
            int row2 = secondLetterPosition.row;
            int column1 = firstLetterPosition.column;
            int column2 = secondLetterPosition.column;

            decipheredText[i] = keyTable[row1][column2];
            decipheredText[i + 1] = keyTable[row2][column1];
        }
    }

    decipheredText[strlen(cipheredText)] = '\0';
}

int main() {

    char *cipheredText = (char *)malloc(strlen(plainText) + 2);
    char *decipheredText = (char *)malloc(strlen(plainText) + 2);
    char **keyTable = (char **)malloc(tableSize * sizeof(char *));

    for (int i = 0; i < tableSize; i ++) {
        keyTable[i] = (char *)malloc(tableSize * sizeof(char) + 1);
    }

    populateKeyTable(keyTable, key);
    cypherTextUsingPlayFair(plainText, cipheredText, keyTable);
    decypherTextUsingPlayFair(cipheredText, decipheredText, keyTable);

    printf("Plain text: %s\n", plainText);
    printf("Ciphered text: %s\n", cipheredText);
    printf("Deciphered text: %s\n", decipheredText);

    return 0;
}