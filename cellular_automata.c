#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "conversions.c"
#include "conway.c"

int advanceGeneration(bool *currentGeneration, int gridWidth, bool *rule);
void printGeneration(bool *generation, int width, FILE *fp);
long promptForInput(char *message, long min, long max);
unsigned char promptForRule();
void clean_stdin();

int main() {
    int width, generations;
    bool rule[8];
    FILE *fp = fopen("savedautomata.txt", "w");
    fclose(fp);
    fp=fopen("savedautomata.txt", "r+");
    if(fp!=NULL) 
    {
        fseek(fp,0,SEEK_END);
        if(ftell(fp)!=0)
        {
            fseek(fp,0,SEEK_SET);
            printf("Would you like to load from file and continue running the automata? (y/n)\n");
            char response;
            scanf("%c", &response);
            if(response=='y')
            {
                char *line=NULL;
                size_t len = 0;
                ssize_t read;

                int lineNumber=0;
                char lastGen[128];
                while((read = getline(&line, &len, fp)) != -1)
                {
                    if(lineNumber==0) 
                    {
                        width = atoi(line);
                    }

                    else if (lineNumber==1) {
                        unsigned char rulenum = (unsigned char) strtol(line,NULL,10);
                        decToBin(rulenum, rule, 8);
                    }
                    else if (lineNumber==2) generations = atoi(line);
                    else if (lineNumber==generations+2) 
                    {
                        strcpy(lastGen,line);
                        printf("%s", line);
                    }
                    else printf("%s", line);

                    lineNumber++;
                }

                bool *newGen = calloc(width, sizeof(bool));
                if (newGen == NULL) return 1;
                for(int i=0; i<width; i++)
                {
                    if(lastGen[i]=='.')
                    {
                        newGen[i] = false;
                    }
                    else if(lastGen[i]=='#')
                    {
                        newGen[i] = true;
                    }
                }
                clean_stdin();
                generations = (int) promptForInput("Insert the amount of additional generations to run", 1, INT_MAX);

                for(int i = 0; i < generations; ++i) {
                    int err = advanceGeneration(newGen, width, rule);
                    if (err != 0) return 1;

                    printGeneration(newGen, width, fp);    
                }
            }
            else if (response=='n') 
            {
                printf("Continuing program...\n");
            }
            else 
            {
                printf("Wrong input.\n");
            }
        }
        fclose(fp);
        fp=fopen("savedautomata.txt", "w");
    }
    else 
    {
        fclose(fp);
        fp=fopen("savedautomata.txt", "w");
    }
    clean_stdin();
    srand(time(NULL));

    //run conway
    while (true) {

        printf("Run conway's game of life (autogenerated and randomised)? (Y/n)\n");
        char choice;
        scanf("%c", &choice);

        if(choice == 'y' || choice == 'Y') {
            runConway();
            break;

        } else if (choice =='n' || choice == 'N') {
            break;

        } else {
            printf("Invalid input. Please enter 'y/Y' or 'n/N'.");
            continue;
        }

        // unexpected error occurred if this line is reached
        break;
    }
    
    printf("Running 1D automaton...\n");
    clean_stdin();
    srand(time(NULL));
    width = (int) promptForInput("Insert the width of the cellular array", 1, INT_MAX);
    fprintf(fp, "%d\n", width);
    int unfinished = 1;
    while(unfinished)
    {
        printf("Would you like to autogenerate rule? (y/n)\n");
        char response;
        scanf("%c", &response);
        if(response=='y')
        {
            clean_stdin();
            int r = rand();
            int newRule = r%255;
            decToBin(newRule, rule, 8);
            printf("Generated rule: %d\n", newRule);
            unfinished=0;
        }
        else if (response=='n') 
        {
            clean_stdin();
            unsigned char rulenum = promptForRule();
            decToBin(rulenum, rule, 8);
            unfinished=0;
        }
        else 
        {
            clean_stdin();
            printf("Wrong input.");
        }
    }
    fprintf(fp, "%hhu\n", binToDec(rule, 8));
    generations = (int) promptForInput("Insert the amount of generations to run", 1, INT_MAX);
    fprintf(fp, "%d\n", generations);
    bool *currentGeneration = calloc(width, sizeof(bool));
    if (currentGeneration == NULL) {
        printf("Internal error: the program could not obtain necessary memory");
        return 1;
    }

    unfinished=1;
    while(unfinished)
    {
    char response;
    clean_stdin();
    printf("Would you like to create first generation? (y/n)\n");
    scanf("%c", &response);
        if(response=='y')
        {
            int correctInput=0;
            char input[width];
            while(!correctInput)
            {
                clean_stdin();
                printf("Enter first generation (e.g. ...##..###):\n");
                scanf("%s",input);
                int length = strlen(input);
                if(width!=length)
                {
                    printf("Input length doesn't match width (width: %d)\n",width);
                }
                else
                {
                    correctInput=1;

                    for(int i=0; i<width; i++)
                    {
                        if(input[i]=='.')
                        {
                            currentGeneration[i] = false;
                        }
                        else if(input[i]=='#')
                        {
                            currentGeneration[i] = true;
                        }
                        else 
                        {
                            printf("Found unrecognised character.\n");
                            correctInput=0;
                            break;
                        }
                    }
                }
            }
            unfinished=0;
        }
        else if (response=='n') 
        {
            clean_stdin();
            unfinished=0;
            currentGeneration[width / 2] = true;
        }
        else 
        {
            clean_stdin();
            printf("Wrong input.");
        }
    }
    
    printGeneration(currentGeneration, width, fp);    

    for(int i = 0; i < generations; ++i) {
        int err = advanceGeneration(currentGeneration, width, rule);
        if (err != 0) return 1;

        printGeneration(currentGeneration, width, fp);    
    }
    free(currentGeneration);

    fclose(fp);
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
        if(i == 0 && currentGeneration[gridWidth - 1])      parentsConfig |= 4; //0b100
        if(currentGeneration[i])                            parentsConfig |= 2; //0b010
        if(i < gridWidth - 1 && currentGeneration[i + 1])   parentsConfig |= 1; //0b001
        if(i == gridWidth - 1 && currentGeneration[0])      parentsConfig |= 1; //0b001


       nextGeneration[i] = rule[parentsConfig];
    }
    memcpy(currentGeneration, nextGeneration, gridWidth * sizeof(bool));
    free(nextGeneration);
    return 0;
}

void printGeneration(bool *generation, int width, FILE *fp) {
    for(int i = 0; i < width; ++i) {
        if(generation[i]) {
           printf("#");
           fprintf(fp, "#");
        } else {
           printf(".");
           fprintf(fp, ".");
        }
    }
    printf("\n");
    fprintf(fp, "\n");
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

void clean_stdin(void)
{
    int c;
    do 
    {
        c = getchar();
    } while (c != '\n' && c != EOF);
}
