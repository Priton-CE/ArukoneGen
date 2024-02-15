#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "arukone.h"

void print_puzzle(int** puzzle, int dimensions) {
    for (int i = 0; i < dimensions; i++) {
        for (int j = 0; j < dimensions; j++) {
            printf("%i ", puzzle[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("No size (at least 2) passed!\n");
        return 1;
    }

    // determine dimension of the field
    int dimensions = atoi(argv[1]);
    if (dimensions < 2) {
        // field_size == 0 -> too small or ERROR
        // field_size == 1 -> too small
        printf("No valid size (at least 2) passed!\n");
        return 1;
    }

    // determine number of number pairs
    int pair_count = dimensions / 2 + dimensions / 3;

    // create puzzle variable and mark it as empty
    struct Puzzle puzzle;
    puzzle.field = NULL;

    // attempt to generate forever
    int last_seed = 1;
    bool generation_success = false;
    while (!generation_success) {
        int seed = (int) time(NULL);
        seed = seed + last_seed;
        last_seed = seed;
        printf("SEED: %i\n", seed);

        // make sure puzzle is empty to not leak memory
        if (puzzle.field != NULL) {
            deallocate_puzzle(puzzle);
        }

        // allocate memory
        puzzle = allocate_puzzle(dimensions, pair_count);
        puzzle.seed = seed; // Only needed for debug prints. Could be removed.

        // generate puzzle (populate puzzle struct) with current time as seed
        generation_success = generate_puzzle(puzzle, seed);
    }

    // DEBUG
    printf("%i\n", dimensions);
    printf("%i\n", pair_count);
    print_puzzle(puzzle.field, puzzle.dimensions);

    // free memory
    deallocate_puzzle(puzzle);
    return 0;
}
