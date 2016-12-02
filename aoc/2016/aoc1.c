#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

enum Heading { NORTH, EAST, SOUTH, WEST };
enum Direction { LEFT = -1, RIGHT = 1 };
struct Leg { enum Direction direction; int distance; };
struct Coords { int x, y; };
struct TraceEntry { struct Coords coords; struct TraceEntry* next; };

static
unsigned int
manhattan_distance(struct Coords coords)
{
    return abs(coords.x) + abs(coords.y);
}

static
size_t
hash_coords(struct Coords coords)
{
    return (size_t) coords.x ^ coords.y;
}

static
bool
been_there(struct Coords coords)
{
    enum { BUCKET_COUNT = 256U };
    static struct TraceEntry* buckets[BUCKET_COUNT];

    if (coords.x == 0 && coords.y == 0)
      return true; // we're back where we started

    size_t idx = hash_coords(coords) % BUCKET_COUNT;
    struct TraceEntry* entry = buckets[idx],
                     * prev = NULL;
    while (entry != NULL)
    {
        if (entry->coords.x == coords.x && entry->coords.y == coords.y)
            return true; // hey, I see our footprints in the snow!

        prev = entry;
        entry = entry->next;
    }


    entry = calloc(1, sizeof *entry);
    entry->coords = coords;

    if (prev != NULL)
        prev->next = entry;
    else
        buckets[idx] = entry;

    return false;
}

static
enum Heading
turn(enum Heading heading, enum Direction direction)
{
    return heading + direction & 3;
}

static
struct Coords
walk(struct Coords coords, enum Heading heading, int distance)
{
    if (heading & 2)
    {
        if (heading & 1)
            coords.x -= distance;
        else
            coords.y -= distance;
    }
    else
    {
        if (heading & 1)
            coords.x += distance;
        else
            coords.y += distance;
    }

    return coords;
}

static
struct Coords
step(struct Coords coords, enum Heading heading)
{
    return walk(coords, heading, 1);
}

static
struct Coords
move(struct Coords coords, enum Heading heading, int distance)
{
    static bool crossed_tracks = false;

    while (!crossed_tracks && distance--)
    {
        coords = step(coords, heading);

        if (been_there(coords))
        {
            printf("Easter Bunny HQ is %u blocks away.\n", manhattan_distance(coords));
            crossed_tracks = true;
        }
    }

    if (crossed_tracks && distance)
    {   // we no longer need to track our steps, so let's speed things up a bit.
        coords = walk(coords, heading, distance);
    }

    return coords;
}

static
struct Leg
parse_instr(void)
{
    struct Leg leg;
    char dircode;
    int readstatus = scanf("%c%d, ", &dircode, &leg.distance);
    if (readstatus == 1 && dircode == '\n')
    {   // that pesky LF at the end of the input file...
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

    return leg;
}

static
struct Coords
follow_directions(void)
{
    struct Coords coords = { 0 };
    enum Heading heading = NORTH;

    while (!feof(stdin))
    {
        struct Leg leg = parse_instr();
        heading = turn(heading, leg.direction);
        coords = move(coords, heading, leg.distance);
    }

    if (ferror(stdin))
    {
        perror("error reading input");
        abort();
    }

    return coords;
}

int
main(void)
{
    struct Coords final = follow_directions();
    printf("Final destination is %u blocks away.\n", manhattan_distance(final));

    return EXIT_SUCCESS;
}
