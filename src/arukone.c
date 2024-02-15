#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "arukone.h"

struct Puzzle allocate_puzzle(int dimensions, int pair_count) {
    // allocate 2d array
    int** memory = malloc(dimensions * sizeof(int*));
    for (int i = 0; i < dimensions; i++) {
        memory[i] = malloc(dimensions * sizeof(int));
    }

    // create Puzzle struct
    struct Puzzle puzzle;
    puzzle.dimensions = dimensions;
    puzzle.pair_count = pair_count;
    puzzle.field = memory;

    return puzzle;
}
void deallocate_puzzle(struct Puzzle puzzle) {
    // free 2d array
    for (int i = 0; i < puzzle.dimensions; i++) {
        free(puzzle.field[i]);
    }
    free(puzzle.field);
}

// helper function to generate a random number
// with inclusive limit
int random_limit(int upper) {
    return rand() % (upper + 1);
}

/////////////////////// DEBUG ///////////////////////
// uncomment for debug printing
//#define DEBUG_PRINT

// helper function to print the field and paths
// without it shifting due to the negatvie values
// of the paths
void dprint_puzzle(int** puzzle, int dimensions) {
    for (int i = 0; i < dimensions; i++) {
        for (int j = 0; j < dimensions; j++) {
            if (puzzle[i][j] >= 0)
                printf(" %i ", puzzle[i][j]);
            else
                printf("%i ", puzzle[i][j]);
        }
        printf("\n");
    }
}

/////////////////////// ALGORITHM ///////////////////////

// Vector2 struct to help storing coordinates and
// direction steps
struct Vector2 {
    int x;
    int y;
};
typedef struct Vector2 Vector2;

// Datatype that represents a direction.
// (Could also be solved via enums.)
#define NO_DIRECTION -1
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
typedef char Direction;

/* Datatype that holds a list of marked directions.
 * 
 * 1 = direction is marked (for example "blocked")
 * 0 = direction is unmarked (for example "free")
 *
 * char (8 bits):
 * 0 0 0 0|0 0 0 0
 * no use |L D R U
 */
#define SET_BITS(n) (1<<(n))
#define CHECK_BIT(v, p) ((v) & (1<<(p)))
typedef char DirectionList;

// determine walls and other paths around a location
DirectionList find_blocked_directions(struct Puzzle puzzle, Vector2 position) {
    DirectionList blacklist = 0;

    // check surroundings
    for (int i = 0; i < 4; i++) {
        Vector2 test_position;
        test_position.x = position.x;
        test_position.y = position.y;
        test_position.x += (i == RIGHT ? 1 : 0);
        test_position.x += (i == LEFT ? -1 : 0);
        test_position.y += (i == UP ? -1 : 0);
        test_position.y += (i == DOWN ? 1 : 0);

        // check for out of bounds (walls)
        if (test_position.x < 0)                        blacklist = blacklist | SET_BITS(LEFT);
        if (test_position.x > puzzle.dimensions - 1)    blacklist = blacklist | SET_BITS(RIGHT);
        if (test_position.y < 0)                        blacklist = blacklist | SET_BITS(UP);
        if (test_position.y > puzzle.dimensions - 1)    blacklist = blacklist | SET_BITS(DOWN);
    }

    // check for non 0 (paths)
    if (!CHECK_BIT(blacklist, LEFT) && puzzle.field[position.y][position.x - 1] != 0) blacklist = blacklist | SET_BITS(LEFT);
    if (!CHECK_BIT(blacklist, RIGHT)&& puzzle.field[position.y][position.x + 1] != 0) blacklist = blacklist | SET_BITS(RIGHT);
    if (!CHECK_BIT(blacklist, UP)   && puzzle.field[position.y - 1][position.x] != 0) blacklist = blacklist | SET_BITS(UP);
    if (!CHECK_BIT(blacklist, DOWN) && puzzle.field[position.y + 1][position.x] != 0) blacklist = blacklist | SET_BITS(DOWN);

    return blacklist;
}

// find a random direction that is not blocked by walls or paths
Direction find_random_direction(struct Puzzle puzzle, Vector2 position) {
    DirectionList blacklist = find_blocked_directions(puzzle, position);

    // attempt 4 times (1 for every direction) to find a valid direction
    Direction direction = NO_DIRECTION;
    Direction direction_proposal = random_limit(3);
    for (int i = 0; i < 4; i++) {
        if (direction_proposal == UP) {
            if (CHECK_BIT(blacklist, UP)) direction_proposal++;
            else {
                direction = direction_proposal;
                break;
            }
        }
        if (direction_proposal == RIGHT) {
            if (CHECK_BIT(blacklist, RIGHT)) direction_proposal++;
            else {
                direction = direction_proposal;
                break;
            }
        }
        if (direction_proposal == DOWN) {
            if (CHECK_BIT(blacklist, DOWN)) direction_proposal++;
            else {
                direction = direction_proposal;
                break;
            }
        }
        if (direction_proposal == LEFT) {
            if (CHECK_BIT(blacklist, LEFT)) direction_proposal = UP;
            else {
                direction = direction_proposal;
                break;
            }
        }
        else printf("ERROR: Unexpected direction randomly generated!\n");
    }

    return direction;
}

// convert a direction to a vector (for example to use for stepping)
Vector2 direction_to_vec(Direction direction) {
    Vector2 v;
    v.x = 0;
    v.y = 0;
    if (direction == UP)    v.y = -1;
    if (direction == RIGHT) v.x = +1;
    if (direction == DOWN)  v.y = +1;
    if (direction == LEFT)  v.x = -1;
    return v;
}

// main algorithm
bool generate_puzzle(struct Puzzle puzzle, unsigned int seed) {
    srand(seed);

    // init field
    for (int i = 0; i < puzzle.dimensions; i++) {
        for (int j = 0; j < puzzle.dimensions; j++) {
            puzzle.field[i][j] = 0;
        }
    }

    // generate start positions
    Vector2* start_positions = malloc(puzzle.pair_count * sizeof(struct Vector2));
    for (int i = 0; i < puzzle.pair_count; i++) {
        // CAUTION: Infinite Loop Hazard!
        // TODO: Find alternative (for loop or similar)
        while (1) {
            int x = random_limit(puzzle.dimensions - 1);
            int y = random_limit(puzzle.dimensions - 1);
            if (puzzle.field[y][x] == 0) {
                // represent data in field
                puzzle.field[y][x] = i + 1;
                start_positions[i].x = x;
                start_positions[i].y = y;
                break;
            }
        }
    }

    // generate maximum segments for each number_pair
    int* max_path_segments = malloc(puzzle.pair_count * sizeof(int));
    for (int i = 0; i < puzzle.pair_count; i++) {
        int desired_segments = 0;
        int chance = random_limit(100);
        if (chance <= ONE_SEGMENT_PROBABILITY)
            desired_segments = 1;
        else if (chance >= 100 - THREE_SEGMENT_PROBABILITY)
            desired_segments = 3;
        else
            desired_segments = 2;

        max_path_segments[i] = desired_segments;
    }


    // generate paths
    for (int number_pair = 0; number_pair < puzzle.pair_count; number_pair++) {

#ifdef DEBUG_PRINT
        printf("DEBUG (%i):\n", puzzle.seed);
        dprint_puzzle(puzzle.field, 6);
        printf("\n\n");
#endif

        Vector2 pos;
        pos.x = start_positions[number_pair].x;
        pos.y = start_positions[number_pair].y;

        // determine start direction
        Direction direction = find_random_direction(puzzle, pos);
        Vector2 step = direction_to_vec(direction);

        if (direction == NO_DIRECTION) {
            // Dead end. Abort generation.
#ifdef DEBUG_PRINT
            printf("START LOCATION BLOCKED\n");
#endif
            free(max_path_segments);
            return false;
        }

        // generate path for this pair
        int element_count = 0;
        for (int n = 0; n < puzzle.dimensions; n++) { // for loop to ensure we do not run into infinite loops due to bugs

#ifdef DEBUG_PRINT
            printf("DEBUG (%i):\n", puzzle.seed);
            dprint_puzzle(puzzle.field, 6);
            printf("\n\n");
#endif

            // check if the path needs to change direction due to a wall or path
            DirectionList blocked_directions = find_blocked_directions(puzzle, pos);
            if (CHECK_BIT(blocked_directions, direction)) {
                // blockage ahead
                // determine new direction
                direction = find_random_direction(puzzle, pos);
                step = direction_to_vec(direction);

                element_count++; // one element has been completed
                n = 0; // reset step counter
            }

            // check if the (potentially new) direction is invalid or we have too many segments
            if (direction == NO_DIRECTION || element_count >= max_path_segments[number_pair]) {
                // dead end or we have enough segments
                // finish the path and end the path generation loop to proceed to the next pair
#ifdef DEBUG_PRINT
                printf("PATH ENDED\n");
#endif
                puzzle.field[pos.y][pos.x] = number_pair + 1;
                break;
            }

            // direction of movement is unobstructed and we have not reached
            // the segment limit yet

            // step
            pos.x += step.x;
            pos.y += step.y;

            // mark path
            puzzle.field[pos.y][pos.x] = -(number_pair + 1);
        }
    }

    // remove path lines
    for (int i = 0; i < puzzle.dimensions; i++) {
        for (int j = 0; j < puzzle.dimensions; j++) {
            if (puzzle.field[j][i] < 0)
                puzzle.field[j][i] = 0;
        }
    }

    free(max_path_segments);
    return true;
}