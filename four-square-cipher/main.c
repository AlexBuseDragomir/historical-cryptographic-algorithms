// AUTHOR: BUSE-DRAGOMIR ALEXANDRU, CEN4S1A
// FOUR SQUARE CIPHER AND DECIPHER; CREATED 27.11.2019

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// alphabet without the letter j
const char* reducedAlphabet = "abcdefghiklmnopqrstuvwxyz";
// english alphabet
const char* alphabet = "abcdefghijklmnopqrstuvwxyz";

const int alphabetSize = 26;
const int squareSize = 5;

char *firstKey = "alexandru";
char *secondKey = "busedragomir";
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

void generateAlphabetTable(char **alphabetTable) {
    for (int i = 0; i < squareSize; i ++) {
        for (int j = 0; j < squareSize; j ++) {
            alphabetTable[i][j] = reducedAlphabet[i * squareSize + j];
        }
    }
}

// used for testing
void printKeyTable(char **keyTable) {
    for (int i = 0; i < squareSize; i ++) {
        for (int j = 0; j < squareSize; j ++) {
            printf("%c ", keyTable[i][j]);
        }
        printf("\n");
    }
}

// used for testing
void printFullKeyTable(char **fullKeyTable) {
    for (int i = 0; i < 2 * squareSize; i ++) {
        for (int j = 0; j < 2 * squareSize; j ++) {
            if (i == squareSize && j == 0) {
                printf("\n");
                printf("%c ", fullKeyTable[i][j]);
            } else if (j == squareSize) {
                printf(" %c ", fullKeyTable[i][j]);
            } else {
                printf("%c ", fullKeyTable[i][j]);
            }
        }
        printf("\n");
    }
}

void appendCharArrays(char *first, char *second, char *result) {
    strcpy(result, first);
    strcat(result, second);
}

struct position getPositionForLetter(char letter, char **alphabetTable) {
    struct position p;
    p.row = -1;
    p.column = -1;

    for (int i = 0; i < squareSize; i ++) {
        for (int j = 0; j < squareSize; j ++) {
            if (alphabetTable[i][j] == letter && letter == 'j') {
                p.row = i;
                p.column = j;
            }
            if (alphabetTable[i][j] == letter) {
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
    char *remainingLetters = malloc(squareSize * squareSize * sizeof(char) + 1);
    char *keyTableContent = malloc(squareSize * squareSize * sizeof(char) + 1);

    // lowercase the key
    lowercaseText(key, result);
    strcpy(key, result);

    // remove duplicates
    removeDuplicatesFromKey(key, result, remainingLetters);
    strcpy(key, result);

    // we concatenate the two arrays
    appendCharArrays(result, remainingLetters, keyTableContent);

    // we populate the key table
    for (int i = 0; i < squareSize; i ++) {
        for (int j = 0; j < squareSize; j ++) {
            keyTable[i][j] = keyTableContent[i * squareSize + j];
        }
    }
}

// creates the matrix that contains the four squares
void populateFullKeyTable(char **fullKeyTable, char *firstKey, char *secondKey) {

    char **alphabetTable = (char **)malloc(squareSize * sizeof(char *));
    char **firstKeyTable = (char **)malloc(squareSize * sizeof(char *));
    char **secondKeyTable = (char **)malloc(squareSize * sizeof(char *));

    for (int i = 0; i < squareSize; i ++) {
        alphabetTable[i] = (char *)malloc(squareSize * sizeof(char) + 1);
        firstKeyTable[i] = (char *)malloc(squareSize * sizeof(char) + 1);
        secondKeyTable[i] = (char *)malloc(squareSize * sizeof(char) + 1);
    }

    populateKeyTable(firstKeyTable, firstKey);
    populateKeyTable(secondKeyTable, secondKey);
    generateAlphabetTable(alphabetTable);

    // upper left square
    for (int i = 0; i < squareSize; i ++) {
        for (int j = 0; j < squareSize; j ++) {
            fullKeyTable[i][j] = alphabetTable[i][j];
        }
    }

    // upper right square
    for (int i = 0; i < squareSize; i ++) {
        for (int j = squareSize; j < 2 * squareSize; j ++) {
            fullKeyTable[i][j] = firstKeyTable[i][j - squareSize];
        }
    }

    // lower left square
    for (int i = squareSize; i < 2 * squareSize; i ++) {
        for (int j = 0; j < squareSize; j ++) {
            fullKeyTable[i][j] = secondKeyTable[i - squareSize][j];
        }
    }

    // lower right square
    for (int i = squareSize; i < 2 * squareSize; i ++) {
        for (int j = squareSize; j < 2 * squareSize; j ++) {
            fullKeyTable[i][j] = alphabetTable[i - squareSize][j - squareSize];
        }
    }
}

void cipherTextUsingFourSquare(char *plainText, char *ciphered, char **fullKeyTable) {
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

    // populate the alphabet table
    char **alphabetTable = (char **)malloc(squareSize * sizeof(char *));

    for (int i = 0; i < squareSize; i ++) {
        alphabetTable[i] = (char *)malloc(squareSize * sizeof(char) + 1);
    }

    generateAlphabetTable(alphabetTable);

    // the cipher process
    for (unsigned int i = 0; i < strlen(modifiedPlainText); i += 2) {
        struct position firstLetterPosition =
                getPositionForLetter(modifiedPlainText[i], alphabetTable);
        struct position secondLetterPosition =
                getPositionForLetter(modifiedPlainText[i + 1], alphabetTable);

        //change position to be related to the full square
        secondLetterPosition.row += squareSize;
        secondLetterPosition.column += squareSize;

        int row1 = firstLetterPosition.row;
        int row2 = secondLetterPosition.row;
        int column1 = firstLetterPosition.column;
        int column2 = secondLetterPosition.column;

        ciphered[i] = fullKeyTable[row1][column2];
        ciphered[i + 1] = fullKeyTable[row2][column1];
    }

    ciphered[strlen(modifiedPlainText)] = '\0';
}

void decipherTextUsingFourSquare(char *ciphered, char *deciphered, char **fullKeyTable) {
    // populate the key tables
    char **firstKeyTable = (char **)malloc(squareSize * sizeof(char *));
    char **secondKeyTable = (char **)malloc(squareSize * sizeof(char *));

    for (int i = 0; i < squareSize; i ++) {
        firstKeyTable[i] = (char *)malloc(squareSize * sizeof(char) + 1);
        secondKeyTable[i] = (char *)malloc(squareSize * sizeof(char) + 1);
    }

    populateKeyTable(firstKeyTable, firstKey);
    populateKeyTable(secondKeyTable, secondKey);

    // the decipher process
    for (unsigned int i = 0; i < strlen(ciphered); i += 2) {
        struct position firstLetterPosition =
                getPositionForLetter(ciphered[i], firstKeyTable);
        struct position secondLetterPosition =
                getPositionForLetter(ciphered[i + 1], secondKeyTable);

        //change position to be related to the full square
        firstLetterPosition.column += 5;
        secondLetterPosition.row += 5;

        int row1 = firstLetterPosition.row;
        int row2 = secondLetterPosition.row;
        int column1 = firstLetterPosition.column;
        int column2 = secondLetterPosition.column;

        deciphered[i] = fullKeyTable[row1][column2];
        deciphered[i + 1] = fullKeyTable[row2][column1];
    }

    deciphered[strlen(ciphered)] = '\0';
}

int main() {

    char *cipheredText = (char *)malloc(strlen(plainText) + 2);
    char *decipheredText = (char *)malloc(strlen(plainText) + 2);

    char **fullKeyTable = (char **)malloc(2 * squareSize * sizeof(char *));

    for (int i = 0; i < 2 * squareSize; i ++) {
        fullKeyTable[i] = (char *)malloc(2 * squareSize * sizeof(char) + 1);
    }

    populateFullKeyTable(fullKeyTable, firstKey, secondKey);
    printFullKeyTable(fullKeyTable);
    printf("\n\n");

    printf("Plain text: %s\n", plainText);
    cipherTextUsingFourSquare(plainText, cipheredText, fullKeyTable);
    printf("Ciphered text: %s\n", cipheredText);
    decipherTextUsingFourSquare(cipheredText, decipheredText, fullKeyTable);
    printf("Deciphered text: %s\n", decipheredText);

    return 0;
}