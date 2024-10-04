// AUTHOR: BUSE-DRAGOMIR ALEXANDRU, CEN4S1A
// DATE 13.01.2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// we use a default period
// in this case, the same from
// the laboratory
const int period = 5;
const int alphabetSize = 26;
const int tableSize = 3;

char *key = "lemonjuice";
char *plainText = "enemyshipsareattackingtheportaftereleven";

struct position {
    int table;
    int row;
    int column;
};

// utility used in testing process
void printIntArray(int *array, int size) {
    for (int i = 0; i < size; i ++) {
        printf("%d ", array[i]);
    }
}

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

void appendThreeArrays(const int *first, const int *second, const int *third, int size, int *result) {
    for (int i = 0; i < size; i ++) {
        result[i] = first[i];
    }
    for (int i = size; i < 2 * size; i ++) {
        result[i] = second[i - size];
    }
    for (int i = 2 * size; i < 3 * size; i ++) {
        result[i] = third[i - 2 * size];
    }
}

void splitArrayInThree(const int *input, int *first, int *second, int *third, int initialSize) {
    int range = initialSize / 3;

    for (int i = 0; i < range; i ++) {
        first[i] = input[i];
    }

    for (int i = range; i < range * 2; i ++) {
        second[i - range] = input[i];
    }

    for (int i = range * 2; i < range * 3; i ++) {
        third[i - range * 2] = input[i];
    }
}

void appendCharArrays(char *first, char *second, char *result) {
    strcpy(result, first);
    strcat(result, second);
}

int getKeyTableNumber(char **keyTable1, char **keyTable2, char **keyTable3, char letter) {
    // table 1
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            if (keyTable1[i][j] == letter) {
                return 1;
            }
        }
    }

    // table 2
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            if (keyTable2[i][j] == letter) {
                return 2;
            }
        }
    }

    // table 3
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            if (keyTable3[i][j] == letter) {
                return 3;
            }
        }
    }

    return -1;
}

struct position getPositionForLetter(char letter, char **keyTable1,
        char **keyTable2, char **keyTable3) {

    struct position p;
    p.table = -1;
    p.row = -1;
    p.column = -1;

    int keyTablePosition = getKeyTableNumber(keyTable1, keyTable2, keyTable3, letter);

    switch(keyTablePosition) {
        case 1: {

            p.table = 1;

            for (int i = 0; i < tableSize; i++) {
                for (int j = 0; j < tableSize; j++) {
                    if (keyTable1[i][j] == letter) {
                        p.row = i;
                        p.column = j;
                    }
                }
            }
            return p;
        }
        case 2: {

            p.table = 2;

            for (int i = 0; i < tableSize; i++) {
                for (int j = 0; j < tableSize; j++) {
                    if (keyTable2[i][j] == letter) {
                        p.row = i;
                        p.column = j;
                    }
                }
            }
            return p;
        }
        case 3: {

            p.table = 3;

            for (int i = 0; i < tableSize; i++) {
                for (int j = 0; j < tableSize; j++) {
                    if (keyTable3[i][j] == letter) {
                        p.row = i;
                        p.column = j;
                    }
                }
            }
            return p;
        }
        default:
            return p;
    }
}

// removes duplicates
void removeDuplicatesFromKey(char *key, char *result, char *remainingLetters) {
    // we will use a appearance array
    int *letterAppearances = malloc(alphabetSize * sizeof(int) + 1);

    // initialize the letter appearances to 0
    initializeArrayWithValue(letterAppearances, alphabetSize, 0);

    int counter = 0;
    char currentLetter;

    for (unsigned int i = 0; i < strlen(key); i ++) {
        currentLetter = key[i];

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
        if (letterAppearances[k] == 0) {
            remainingLetters[counter] = (char)('a' + k);
            counter ++;
        }
    }

    remainingLetters[counter] = '.';
    remainingLetters[counter + 1] = '\0';
}

void populateKeyTables(char **keyTable1, char **keyTable2, char **keyTable3, char *key) {

    char *result = malloc(strlen(key) * sizeof(char) + 2);
    char *remainingLetters = malloc(tableSize * tableSize * tableSize * sizeof(char) + 1);
    char *keyTablesContent = malloc(tableSize * tableSize * tableSize * sizeof(char) + 1);

    // lowercase the key
    lowercaseText(key, result);
    strcpy(key, result);

    // remove duplicates
    removeDuplicatesFromKey(key, result, remainingLetters);
    strcpy(key, result);

    // we concatenate the two arrays
    appendCharArrays(result, remainingLetters, keyTablesContent);

    // we populate the key tables
    int iterator = 0;

    // table 1
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            keyTable1[i][j] = keyTablesContent[iterator];
            iterator ++;
        }
    }

    // table 2
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            keyTable2[i][j] = keyTablesContent[iterator];
            iterator ++;
        }
    }

    // table 3
    for (int i = 0; i < tableSize; i ++) {
        for (int j = 0; j < tableSize; j ++) {
            keyTable3[i][j] = keyTablesContent[iterator];
            iterator ++;
        }
    }
}

void encodeTrifidCipher(char *key, char *plainText, char *cipheredText) {

    char **keyTable1 = (char **)malloc(tableSize * sizeof(char *));
    char **keyTable2 = (char **)malloc(tableSize * sizeof(char *));
    char **keyTable3 = (char **)malloc(tableSize * sizeof(char *));

    for (int i = 0; i < tableSize; i ++) {
        keyTable1[i] = (char *)malloc(tableSize * sizeof(char) + 1);
        keyTable2[i] = (char *)malloc(tableSize * sizeof(char) + 1);
        keyTable3[i] = (char *)malloc(tableSize * sizeof(char) + 1);
    }

    populateKeyTables(keyTable1, keyTable2, keyTable3, key);

    int completePeriods = strlen(plainText) / period;
    int remainingLetters = strlen(plainText) % period;

    int counter = 0;

    // first for complete pairs
    for (int i = 0; i < completePeriods; i ++) {
        // we use three arrays for encoding
        int *tableCoordinates = malloc(period * sizeof(int));
        int *lineCoordinates = malloc(period * sizeof(int));
        int *columnCoordinates = malloc(period * sizeof(int));

        int startIndex = i * period;
        int endIndex = (i + 1) * period;

        for (int j = startIndex; j < endIndex; j ++) {
            struct position letterPosition =
                    getPositionForLetter(plainText[j], keyTable1, keyTable2, keyTable3);

            lineCoordinates[j - startIndex] = letterPosition.row + 1;
            columnCoordinates[j - startIndex] = letterPosition.column + 1;
            tableCoordinates[j - startIndex] = letterPosition.table;
        }

        int *result = malloc(3 * period * sizeof(int));

        appendThreeArrays(tableCoordinates, lineCoordinates, columnCoordinates, period, result);

        for (int k = 0; k < period * 3; k += 3) {

            int table = result[k];
            int row = result[k + 1];
            int column = result[k + 2];

            switch(table) {
                case 1: {
                    cipheredText[counter] = keyTable1[row - 1][column - 1];
                    counter ++;
                    break;
                }
                case 2: {
                    cipheredText[counter] = keyTable2[row - 1][column - 1];
                    counter ++;
                    break;
                }
                case 3: {
                    cipheredText[counter] = keyTable3[row - 1][column - 1];
                    counter ++;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    // if there are remaining letters
    // do the same for them
    if (remainingLetters != 0) {

        // we use three arrays for encoding
        int *tableCoordinates = malloc(remainingLetters * sizeof(int));
        int *lineCoordinates = malloc(remainingLetters * sizeof(int));
        int *columnCoordinates = malloc(remainingLetters * sizeof(int));

        for (int i = completePeriods * period; i < strlen(plainText); i ++) {
            struct position letterPosition =
                    getPositionForLetter(plainText[i], keyTable1, keyTable2, keyTable3);

            lineCoordinates[i - completePeriods * period] = letterPosition.row + 1;
            columnCoordinates[i - completePeriods * period] = letterPosition.column + 1;
            tableCoordinates[i - completePeriods * period] = letterPosition.table;
        }

        int *result = malloc(3 * remainingLetters * sizeof(int));

        appendThreeArrays(tableCoordinates, lineCoordinates, columnCoordinates, remainingLetters, result);

        for (int k = 0; k < remainingLetters * 3; k += 3) {

            int table = result[k];
            int row = result[k + 1];
            int column = result[k + 2];

            switch(table) {
                case 1: {
                    cipheredText[counter] = keyTable1[row - 1][column - 1];
                    counter ++;
                    break;
                }
                case 2: {
                    cipheredText[counter] = keyTable2[row - 1][column - 1];
                    counter ++;
                    break;
                }
                case 3: {
                    cipheredText[counter] = keyTable3[row - 1][column - 1];
                    counter ++;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    cipheredText[counter] = '\0';
}

void decodeTrifidCipher(char *key, char *cipheredText, char *decipheredText) {

    char **keyTable1 = (char **)malloc(tableSize * sizeof(char *));
    char **keyTable2 = (char **)malloc(tableSize * sizeof(char *));
    char **keyTable3 = (char **)malloc(tableSize * sizeof(char *));

    for (int i = 0; i < tableSize; i ++) {
        keyTable1[i] = (char *)malloc(tableSize * sizeof(char) + 1);
        keyTable2[i] = (char *)malloc(tableSize * sizeof(char) + 1);
        keyTable3[i] = (char *)malloc(tableSize * sizeof(char) + 1);
    }

    populateKeyTables(keyTable1, keyTable2, keyTable3, key);

    int completePeriods = strlen(plainText) / period;
    int remainingLetters = strlen(plainText) % period;

    int counter = 0;

    // first for complete pairs
    for (int i = 0; i < completePeriods; i ++) {
        int *allRows = malloc(3 * period * sizeof(int));

        int startIndex = i * period;
        int endIndex = (i + 1) * period;

        for (int j = startIndex; j < endIndex; j ++) {
            struct position letterPosition =
                    getPositionForLetter(cipheredText[j], keyTable1, keyTable2, keyTable3);

            allRows[(j - startIndex) * 3] = letterPosition.table;
            allRows[(j - startIndex) * 3 + 1] = letterPosition.row + 1;
            allRows[(j - startIndex) * 3 + 2] = letterPosition.column + 1;
        }

        int *lineCoordinates = malloc(remainingLetters * sizeof(int));
        int *columnCoordinates = malloc(remainingLetters * sizeof(int));
        int *tableCoordinates = malloc(remainingLetters * sizeof(int));

        splitArrayInThree(allRows, tableCoordinates, lineCoordinates, columnCoordinates, 3 * period);

        for (int k = 0; k < period; k ++) {

            int table = tableCoordinates[k];
            int row = lineCoordinates[k] - 1;
            int column = columnCoordinates[k] - 1;

            switch(table) {
                case 1: {
                    decipheredText[counter] = keyTable1[row][column];
                    counter ++;
                    break;
                }
                case 2: {
                    decipheredText[counter] = keyTable2[row][column];
                    counter ++;
                    break;
                }
                case 3: {
                    decipheredText[counter] = keyTable3[row][column];
                    counter ++;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    // then for the remaining letters
    // if there are any
    if (remainingLetters != 0) {
        int *allRows = malloc(3 * remainingLetters * sizeof(int));

        for (int i = completePeriods * period; i < strlen(cipheredText); i ++) {
            struct position letterPosition =
                    getPositionForLetter(cipheredText[i], keyTable1, keyTable2, keyTable3);

            allRows[(i - completePeriods * period) * 3] = letterPosition.table;
            allRows[(i - completePeriods * period) * 3 + 1] = letterPosition.row + 1;
            allRows[(i - completePeriods * period) * 3 + 2] = letterPosition.column + 1;
        }

        int *tableCoordinates = malloc(remainingLetters * sizeof(int));
        int *lineCoordinates = malloc(remainingLetters * sizeof(int));
        int *columnCoordinates = malloc(remainingLetters * sizeof(int));

        splitArrayInThree(allRows, tableCoordinates, lineCoordinates, columnCoordinates, 3 * remainingLetters);

        for (int k = 0; k < remainingLetters; k ++) {

            int table = lineCoordinates[k];
            int row = lineCoordinates[k] - 1;
            int column = columnCoordinates[k] - 1;

            switch(table) {
                case 1: {
                    decipheredText[counter] = keyTable1[row][column];
                    counter ++;
                    break;
                }
                case 2: {
                    decipheredText[counter] = keyTable2[row][column];
                    counter ++;
                    break;
                }
                case 3: {
                    decipheredText[counter] = keyTable3[row][column];
                    counter ++;
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }
    decipheredText[counter] = '\0';
}

int main() {

    char *cipheredText = (char *)malloc(strlen(plainText) + 1);
    char *decipheredText = (char *)malloc(strlen(plainText) + 1);

    char **keyTable1 = (char **)malloc(tableSize * sizeof(char *));
    char **keyTable2 = (char **)malloc(tableSize * sizeof(char *));
    char **keyTable3 = (char **)malloc(tableSize * sizeof(char *));

    for (int i = 0; i < tableSize; i ++) {
        keyTable1[i] = (char *)malloc(tableSize * sizeof(char) + 1);
        keyTable2[i] = (char *)malloc(tableSize * sizeof(char) + 1);
        keyTable3[i] = (char *)malloc(tableSize * sizeof(char) + 1);
    }

    populateKeyTables(keyTable1, keyTable2, keyTable3, key);

    encodeTrifidCipher(key, plainText, cipheredText);
    decodeTrifidCipher(key, cipheredText, decipheredText);

    printf("\nSquares for key\n\n");

    printKeyTable(keyTable1, tableSize);
    printf("\n\n");
    printKeyTable(keyTable2, tableSize);
    printf("\n\n");
    printKeyTable(keyTable3, tableSize);
    printf("\n\n");

    printf("\nPlain text: %s\n", plainText);
    printf("Ciphered text: %s\n", cipheredText);
    printf("Deciphered text: %s\n", decipheredText);

    return 0;
}