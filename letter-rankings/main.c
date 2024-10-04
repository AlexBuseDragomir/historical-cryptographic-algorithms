#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// read all the file into a char buffer
void readInputIntoBuffer(const char *fileName, char *buffer) {
    long fileSize = 0;

    FILE *fp = fopen(fileName, "r");

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);

    rewind(fp);

    fread(buffer, (size_t)fileSize, 1, fp);

    buffer[fileSize] = '\0';
}

// get the size of the buffer (full text length)
long getFileSize(const char *filename) {
    long size = 0;

    FILE *fp = fopen(filename, "r");

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    rewind(fp);

    return size;
}

// lowercase all the uppercase letters to normalize text
void lowercaseText(char *buffer) {
    for (unsigned int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            buffer[i] = (char)tolower(buffer[i]);
        }
    }
}

// get a frequency for each letter
void constructFrequencyTable(char *buffer, int *frequencyTable) {
    // set to zero all frequencies for letters
    for (int i = 'a'; i <= 'z'; i ++) {
        frequencyTable[i] = 0;
    }

    for (unsigned int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            frequencyTable[(int)(buffer[i])] ++;
        }
    }
}

void printLetterFrequencies(const int *frequencyTable) {
    int numberOfLetters = 0;
    double sum = 0.0;

    // get the total number of letters
    for (int i = 'a'; i <= 'z'; i++) {
        numberOfLetters += frequencyTable[i];
    }

    for (int i = 'a'; i <= 'z'; i++) {
        double frequency = (double) frequencyTable[i] / (double) numberOfLetters;
        printf("Letter %c with frequency: %f%%\n", (char) i, frequency * 100);
    }
}

int main() {
    const char *fileName1 = "input_english.txt";
    const char *fileName2 = "input_romanian.txt";
    const char *fileName3 = "input_french.txt";
    const char *fileName4 = "input_german.txt";

    long fileSize1 = getFileSize(fileName1);
    long fileSize2 = getFileSize(fileName2);
    long fileSize3 = getFileSize(fileName3);
    long fileSize4 = getFileSize(fileName4);

    char *buffer1 = malloc(fileSize1 * sizeof(char) + 1);
    char *buffer2 = malloc(fileSize2 * sizeof(char) + 1);
    char *buffer3 = malloc(fileSize3 * sizeof(char) + 1);
    char *buffer4 = malloc(fileSize4 * sizeof(char) + 1);

    readInputIntoBuffer(fileName1, buffer1);
    readInputIntoBuffer(fileName2, buffer2);
    readInputIntoBuffer(fileName3, buffer3);
    readInputIntoBuffer(fileName4, buffer4);

    lowercaseText(buffer1);
    lowercaseText(buffer2);
    lowercaseText(buffer3);
    lowercaseText(buffer4);

    // we are interested in frequencies from index 97('a') to 122('z')
    int *frequencyTable1 = (int*)malloc(124 * sizeof(int));
    int *frequencyTable2 = (int*)malloc(124 * sizeof(int));
    int *frequencyTable3 = (int*)malloc(124 * sizeof(int));
    int *frequencyTable4 = (int*)malloc(124 * sizeof(int));

    constructFrequencyTable(buffer1, frequencyTable1);
    constructFrequencyTable(buffer2, frequencyTable2);
    constructFrequencyTable(buffer3, frequencyTable3);
    constructFrequencyTable(buffer4, frequencyTable4);

    printf("\n             ENGLISH\n");
    printLetterFrequencies(frequencyTable1);
    printf("\n             ROMANIAN\n");
    printLetterFrequencies(frequencyTable2);
    printf("\n             FRENCH\n");
    printLetterFrequencies(frequencyTable3);
    printf("\n             GERMAN\n");
    printLetterFrequencies(frequencyTable4);

    return 0;
}