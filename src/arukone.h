#include <stdbool.h>

// probabilities for the desired segement amounts (change as seen fit)
#define ONE_SEGMENT_PROBABILITY 5
#define THREE_SEGMENT_PROBABILITY 50

// Puzzle struct to hold the puzzles information
struct Puzzle {
    int dimensions;
    int pair_count;
    int** field;
    int seed; // Only needed for debug prints. Could be removed
};

// struct allocation functions
struct Puzzle allocate_puzzle(int, int);
void deallocate_puzzle(struct Puzzle);

// generation function
bool generate_puzzle(struct Puzzle, unsigned int);