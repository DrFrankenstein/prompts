#include <stdlib.h>
#include <stdio.h>

enum Heading { NORTH, EAST, SOUTH, WEST };
enum Direction { LEFT = -1, RIGHT = 1 };
struct Leg { enum Direction direction; int distance; };
struct Coords { int x, y; };

enum Heading turn(enum Heading heading, enum Direction direction)
{
    return heading + direction & 3;
}

void move(struct Coords* coords, enum Heading heading, int distance)
{
    int* component = (heading & 1)? &coords->y : &coords->x;
    if (heading & 2)
        distance = -distance;

    *component += distance;

    printf("x = %3d, y = %3d\n", coords->x, coords->y);
}

struct Leg parse_instr(void)
{
    struct Leg leg;
    char dircode;
    int readstatus = scanf(" %c%d, ", &dircode, &leg.distance);
    if (readstatus == EOF)
    {   /* that pesky LF at the end of the input file... */
        leg.distance = 0;
        return leg;
    }

    if (readstatus < 2)
    {
        perror("parse error in input");
        abort();
    }

    switch (dircode)
    {
    case 'L': leg.direction = LEFT; break;
    case 'R': leg.direction = RIGHT; break;
    default: 
        printf("invalid turn direction '%c'\n", dircode);
        abort();
    }

    printf("turn %c, move %3d, ", dircode, leg.distance);

    return leg;
}

struct Coords follow_directions(void)
{
    struct Coords coords = { 0 };
    enum Heading heading = NORTH;

    while (!feof(stdin))
    {
        struct Leg leg = parse_instr();
        heading = turn(heading, leg.direction);
        move(&coords, heading, leg.distance);
    }

    if (ferror(stdin))
    {
        perror("error reading input");
        abort();
    }

    return coords;
}

int main()
{
    struct Coords final = follow_directions();
    int total_blocks = abs(final.x) + abs(final.y);
    printf("Easter Bunny HQ is %d blocks away.\n", total_blocks);

    return EXIT_SUCCESS;
}
