#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

int advanceGeneration(bool currentGeneration[], int gridWidth, unsigned char rule);
void printGeneration(bool generation[], int width);
long promptForInput(char *message, long min, long max);

int main() {
    int width = (int) promptForInput("Insert the width of the cellular array", 1, 201);
    unsigned char rule = (unsigned char) promptForInput("Insert the rule", 0, 255);
    int generations = (int) promptForInput("Insert the amount of generations to run", 1, INT_MAX);

    bool *currentGeneration = calloc(width, sizeof(bool));
    if (currentGeneration == NULL) return 1;

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

int advanceGeneration(bool currentGeneration[], int gridWidth, unsigned char rule) {
    bool *nextGeneration = malloc(gridWidth * sizeof(bool));
    if (nextGeneration == NULL) return 1;

    for(int i = 0; i < gridWidth; ++i) {
        char parentsConfig = 0;

        if(i > 0 && currentGeneration[i - 1])               parentsConfig |= 4; //0b100
        if(currentGeneration[i])                            parentsConfig |= 2; //0b100
        if(i < gridWidth - 1 && currentGeneration[i + 1])   parentsConfig |= 1; //0b100

        char mask = 1 << parentsConfig;

       nextGeneration[i] = !!(rule & mask);
    }
    memcpy(currentGeneration, nextGeneration, gridWidth * sizeof(bool));
    free(nextGeneration);
    return 0;
}

void printGeneration(bool generation[], int width) {
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
    while (true) {
        printf("%s\n", message);
        char buffer[9] = {0};
        fgets(buffer, 9, stdin);

        errno = 0;
        char *end;
        long number = strtol(buffer, &end, 10);
        
        if (buffer == end) {
            printf("ERROR: No input was provided\n");
            continue;
        } else if (errno == ERANGE && number == LONG_MIN) {
            printf ("ERROR: The input provided is too small (minimum input is %ld)\n", min);
            continue;
        } else if (errno == ERANGE && number == LONG_MAX) {
            printf ("ERROR: The input provided is too large (maximum input is %ld)\n", max);
            continue;
        } else if (errno != 0 && number == 0) {
            printf ("ERROR: Invalid input\n");
            continue;
        } else if (errno == 0 && *end != '\n') {
            printf("WARNING: Some of the input could not be interpreted as a number and will be ignored\n");
        }
        
        if(number < min) {
            printf ("ERROR: The input provided is too small (minimum input is %ld)\n", min);
            continue;
        } else if(number > max) {
            printf ("ERROR: The input provided is too large (maximum input is %ld)\n", max);
            continue;
        }
        
        return number;
    }
}

