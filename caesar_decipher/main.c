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

// function based on the one above that shifts
// with a fixed number of positions to the left
void shiftAlphabetToLeftWithPositions(char *alphabet, char *newAlphabet, int positions) {
    int alphabetSize = strlen(alphabet);

    if (positions > -1 && positions < alphabetSize) {
        strcpy(newAlphabet, alphabet);

        for (int i = 0; i < positions; i ++) {
            shiftAlphabetToLeft(newAlphabet);
        }
    } else {
        printf("ERROR: Insert a shift between 0 and %d", strlen(alphabet) - 1);
    }
}

// the algorithm of Caesar decipher, based on shifting
// and getting a form of the text
// only one shift will get to obtain the plain text
void decipherCaesarWithShift(char *cipheredText, char *decipheredText,
        char *alphabet, int shift) {

    int recognised = 0;
    char *shiftedAlphabet = malloc(strlen(alphabet) + 1);

    shiftAlphabetToLeftWithPositions(alphabet, shiftedAlphabet, shift);

    for (unsigned int k = 0; k < strlen(cipheredText); k ++) {
        cipheredText[k] = (char)(tolower(cipheredText[k]));
    }

    for (unsigned int i = 0; i < strlen(cipheredText); i ++) {
        for (unsigned int j = 0; j < strlen(shiftedAlphabet); j ++) {
            if (cipheredText[i] == shiftedAlphabet[j]) {
                decipheredText[i] = alphabet[j];
                recognised = 1;
                break;
            }
        }

        if (recognised == 0) {
            decipheredText[i] = cipheredText[i];
        }

        recognised = 0;
    }

    decipheredText[strlen(cipheredText)] = '\0';
}

int main() {
    char *alphabet = "abcdefghijklmnopqrstuvwxyz";

    // key is +16
    char *cipheredText = "myjxekj tulyqjyed vhec jxu dehc fhewhuii yi dej feiiyrbu";
    char *decipheredText = malloc(strlen(cipheredText) + 1);

    printf("\nCiphered text: %s\n\n", cipheredText);

    for (unsigned int i = 0; i < strlen(alphabet); i ++) {
        decipherCaesarWithShift(cipheredText, decipheredText, alphabet, i);
        printf("Caesar with shift %d: %s\n", i, decipheredText);
    }

    return 0;
}

