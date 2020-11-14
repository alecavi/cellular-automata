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

// You can write down the state of a cell's parents as a string of 3 bits (a 3-bit unsigned integer, if you will).
// 101, for example (left and right parents alive, central parent dead).
// Conventionally, they're ordered from the biggest to the smallest: 111 110 101 100 011 010 001 000.
// Now, if you write down, in their order, the 8 bits that make up a rule, you can draw a 1-1 correspondence
// between each of these bits and each of the parent's states.
// For example, let's look at the diagram for rule 30 (00011110 in binary), below
//
// 111 110 101 100 011 010 001 000
// 0   0   0   1   1   1   1   0  
//
// You can use this as a lookup table. For example, if your parents' state is 110, that means that 
// your own state should be 0, as 110 is paired with 0 in the table.
//
// The code below looks at every cell's 3 parents, and sets the corresponding bit in a 3-bit integer (stored in an 8-bit one).
// This works because, for example, if your left parent is alive, we know for sure 
// that the state of your parents looks like 1xx
// (We also need to handle the leftmost and rightmost cells specially, because accessing their left and right parents respectively
// would go out of the array's bounds. The way they're handled here simply behaves as if there was a dead cell out of bounds)
// We can use bitwise ORs to set individual bits. 
//
// By doing the above, we create a number between 0 (000) and 7 (111) inclusive.
// Then, we want to know if the bit positioned at that number (counting from the right,
// considering the last bit as positioned at 0) is set or not. To do that, we can use a bitwise AND:
// To know whether bit 2 is set in N, one can use N & 0000_0100. It yields 0 if the bit is not set, and 0000_0100 if it is.
// To produce the correct mask for the bitwise AND, we simply bitshift 1 by the appropriate number. 
// For example, 0000_0001 << 2 yields 0000_0100, which is the mask we need to look at bit 2.
int advanceGeneration(bool currentGeneration[], int gridWidth, unsigned char rule) {
    bool *nextGeneration = malloc(gridWidth * sizeof(bool));
    if (nextGeneration == NULL) return 1;

    for(int i = 0; i < gridWidth; ++i) {
        char parentsConfig = 0;

        if(i > 0 && currentGeneration[i - 1])               parentsConfig |= 4; //0b100
        if(currentGeneration[i])                            parentsConfig |= 2; //0b010
        if(i < gridWidth - 1 && currentGeneration[i + 1])   parentsConfig |= 1; //0b001

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

