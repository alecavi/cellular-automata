#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "conversions.c"

int advanceGeneration(bool *currentGeneration, int gridWidth, bool *rule);
void printGeneration(bool *generation, int width);
long promptForInput(char *message, long min, long max);
unsigned char promptForRule();

int main() {
    int width = (int) promptForInput("Insert the width of the cellular array", 1, INT_MAX);
    unsigned char rulenum = promptForRule();
    int generations = (int) promptForInput("Insert the amount of generations to run", 1, INT_MAX);

    bool rule[8];
    decToBin(rulenum, rule, 8);

    bool *currentGeneration = calloc(width, sizeof(bool));
    if (currentGeneration == NULL) {
        printf("Internal error: the program could not obtain necessary memory");
        return 1;
    }

    currentGeneration[width / 2 + 1] = true;
    
    printGeneration(currentGeneration, width);    

    for(int i = 0; i < generations; ++i) {
        int err = advanceGeneration(currentGeneration, width, rule);
        if (err != 0) return 1;

        printGeneration(currentGeneration, width);    
    }
    free(currentGeneration);

    return 0;

}

int advanceGeneration(bool *currentGeneration, int gridWidth, bool *rule) {
    bool *nextGeneration = malloc(gridWidth * sizeof(bool));
    if (nextGeneration == NULL) {
        printf("Internal error: the program could not obtain necessary memory");
        return 1;
    }

    for(int i = 0; i < gridWidth; ++i) {
        int parentsConfig = 0;

        if(i > 0 && currentGeneration[i - 1])               parentsConfig |= 4; //0b100
        if(currentGeneration[i])                            parentsConfig |= 2; //0b010
        if(i < gridWidth - 1 && currentGeneration[i + 1])   parentsConfig |= 1; //0b001


       nextGeneration[i] = rule[parentsConfig];
    }
    memcpy(currentGeneration, nextGeneration, gridWidth * sizeof(bool));
    free(nextGeneration);
    return 0;
}

void printGeneration(bool *generation, int width) {
    for(int i = 0; i < width; ++i) {
        if(generation[i]) {
           printf("#");
        } else {
           printf(".");
        }
    }
    printf("\n");
}

long promptForInput(char *message, long min, long max) {
    while(true) {
        printf("%s\n", message);
        char buffer[128] = {'\0'};
        fgets(buffer, 128, stdin);

        errno = 0;
        char *endptr;
        long numOrErr = strtol(buffer, &endptr, 10);
        
        if(buffer[0] == '\0') {
            fprintf(stderr, "ERROR: No number was provided\n");
        } else if(errno == ERANGE) {
            if(numOrErr == LONG_MIN) fprintf(stderr, "ERROR: The input provided is too small (minimum input is %lu\n", min);
            else fprintf(stderr, "ERROR: The input provided is too large (maximum input is %lu\n", max);
        } else if(errno != 0) {
            fprintf(stderr, "ERROR: Invalid input\n");
        } else if(*endptr != '\0' && *endptr != '\n') {
            fprintf(stderr, "ERROR: Some of the input is valid, but some is not\n");
        } else if(numOrErr < min) {
            fprintf(stderr, "ERROR: The input provided is too small (minimum input is %lu\n", min);
        } else if(numOrErr > max) {
            fprintf(stderr, "ERROR: The input provided is too large (maximum input is %lu\n", max);
        } else {
            return numOrErr;
        }
    }
}

unsigned char promptForRule() {
    while(true) {
        printf("Input the rule. Accepted formats are \"129\", \"0b10011001\", and \"0xABCD\"\n");
        char buffer[128] = {'\0'}; 
        fgets(buffer, 128,  stdin); 
    
        int base;
        char *bufferStart;
        if(buffer[0] == '0') {
            if(tolower(buffer[1]) == 'b') {
                base = 2;
            } else if(tolower(buffer[1]) == 'x') {
                base = 16;
            }
            bufferStart = &buffer[2];
        } else {
            base = 10;
            bufferStart = buffer;
        }

        errno = 0;
        char *endptr;
        long numOrErr = strtol(bufferStart, &endptr, base);

        if(*bufferStart == '\0') {
            fprintf(stderr, "ERROR: No number was provided\n");
        } else if(errno == ERANGE) {
            if(numOrErr == LONG_MIN) fprintf(stderr, "ERROR: The input provided is too small (minimum input is 0\n");
            else fprintf(stderr, "ERROR: The input provided is too large (maximum input is 255\n");
        } else if(errno != 0) {
            fprintf(stderr, "ERROR: Invalid input\n");
        } else if(*endptr != '\0' && *endptr != '\n') {
            fprintf(stderr, "ERROR: Some of the input is valid, but some is not\n");
        } else if(numOrErr < 0) {
            fprintf(stderr, "ERROR: The input provided is too small (minimum input is 0\n");
        } else if(numOrErr > 255) {
            fprintf(stderr, "ERROR: The input provided is too large (maximum input is 255\n");
        } else {
            return (unsigned char) numOrErr;
        }
    }   
}
