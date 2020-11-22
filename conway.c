#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// defines the SIZE of the grid
#define SIZE 15

typedef struct {
    int x;
    int y;
    bool isAlive;
} Cell;

// randomises which cells are alive, with a given chance of being alive (between 0 and 1)
void randomiseCells(Cell cells[][SIZE], float percentChance) {

    // uses system time as a seed for the rand() function
    time_t t;
    srand((unsigned) time(&t));

    int randX;
    int randY;

    for(int i = 0; i < (SIZE * SIZE * percentChance); i++) {
        
        randX = rand() % SIZE; // random X value between 0 and SIZE
        randY = rand() % SIZE; // random Y value between 0 and SIZE

        if(cells[randX][randY].isAlive) { // if cell is already alive, reset and iterate again
            i--;
            continue;
        
        } else {

            cells[randX][randY].isAlive = true;
        }
    }
}

// initialises the cells' values
void initCells(Cell cells[][SIZE]) {

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){

            cells[i][j].x = i;
            cells[i][j].y = j;
            cells[i][j].isAlive = false;

        }
    }
}

// counts the number of living cells around a cell
int checkNeighbours(Cell cells[][SIZE], int x, int y) {

    int aliveCells = 0;

    for(int i = x - 1; i <= x + 1; i++) {
        for(int j = y - 1; j <= y + 1; j++) {

            if (i == -1 || j == -1 || i == SIZE || j == SIZE) continue; // if out of bounds
            if (x == i && y == j) continue; // if cell is the one bieng checked

            if(cells[i][j].isAlive) {
                aliveCells++;
            }
        }
    }

    return aliveCells;
}

// copies the states of all cells into another grid
void copyCells(Cell cellsToCopy[][SIZE], Cell loc[][SIZE]) {

    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {

            loc[i][j].x = cellsToCopy[i][j].x;
            loc[i][j].y = cellsToCopy[i][j].y;
            loc[i][j].isAlive = cellsToCopy[i][j].isAlive;

        }
    }

}

// creates the next generation of cells
void getNextGeneration(Cell cells[][SIZE]) {

    Cell temp[SIZE][SIZE];
    copyCells(cells, temp); // create a copy so the next generation is accurate

    int neighbourCount;

    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {

            neighbourCount = checkNeighbours(temp, i, j);

            if(temp[i][j].isAlive) {

                if((neighbourCount < 2 || neighbourCount > 3)) {
                    cells[i][j].isAlive = false;
                }
            
            } else {
                if(neighbourCount == 3) {
                    cells[i][j].isAlive = true;
                }
            }
        }
    }


}

// displays all the cell states to the screen
void showCells(Cell cells[][SIZE]) {

    printf("\n");

    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {

            if(cells[i][j].isAlive) {
                printf("* ");
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

// checks if all cells are dead
bool allCellsDead(Cell cells[][SIZE]) {

    int deadCount = 0;
    int deadLim = SIZE * SIZE;

    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {

            if (!cells[i][j].isAlive) {
                deadCount++;
            }
        }
    }

    return deadCount == deadLim;
}

int main() {

    char yn;
    
    Cell cells[SIZE][SIZE];

    initCells(cells);
    randomiseCells(cells, 0.25f);

    while(true) {
        
        showCells(cells);

        if(allCellsDead(cells)){
            printf("All cells are dead. Terminating program.\n\n");
            return 0;
        }

        confirm:

        printf("Go to next iteration? (Y/n) >>>  ");
        yn = getchar();

        while(getchar() != '\n');

        if(yn == 'y' || yn == 'Y') {
            getNextGeneration(cells);
            continue;
        
        } else if (yn == 'n' || yn == 'N') {
            break;
        
        } else {
            goto confirm;
        }
    }

    return 0;
}

