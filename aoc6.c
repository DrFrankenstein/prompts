#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char lights[1000][1000] = {0};

const signed char UNKNOWN = 0;
const signed char TURN_ON = 1;
const signed char TURN_OFF = -1;
const signed char TOGGLE = 2;

struct rect { size_t x1, y1, x2, y2; };

void perform_op(signed char opcode, const struct rect* r)
{
    size_t x, y;

    for (y = r->y1; y <= r->y2; ++y)
        for (x = r->x1; x <= r->x2; ++x)
            if (opcode >= 0 || lights[y][x] != 0)   /* prevent underflow */
                lights[y][x] += opcode;
}

void parse_error(const char* message, FILE* stream)
{
    char garbage[81];
    fgets(garbage, 80, stream);

    fprintf(stderr, "Parse error: %s at offset %ld near '%s'",
        message, ftell(stream), garbage);

    exit(EXIT_FAILURE);
}

signed char parse_command(FILE* stream)
{
    char command[8] = {0};
    long off = ftell(stream);

    (void) fscanf(stream, "%7s", command);
    if (strcmp(command, "toggle") == 0)
        return TOGGLE;
    if (strcmp(command, "turn") == 0)
    {
        (void) fscanf(stream, "%3s", command);
        if (strcmp(command, "on") == 0)
            return TURN_ON;
        if (strcmp(command, "off") == 0)
            return TURN_OFF;
    }

    /* if we got nothing, then we've read trailing whitespace in the input.
            it's valid. */
    if (command[0] != '\0')
    {
        fseek(stream, off, SEEK_SET);
        parse_error("invalid command", stream);
    }

    return UNKNOWN;
}

void parse_coords(FILE* stream, struct rect* r)
{
    if (fscanf(stream, "%3u,%3u through %3u,%3u", &r->x1, &r->y1, &r->x2, &r->y2) < 4)
        parse_error("cannot read coordinates", stream);
}

void step(FILE* stream)
{
    struct rect r;
    signed char op = parse_command(stream);

    if (op == UNKNOWN)
        return;

    parse_coords(stream, &r);
    perform_op(op, &r);
}

void exec(FILE* stream)
{
    while (!feof(stream))
        step(stream);

    return;
}

long measure_brightness(void)
{
    long total = 0;
    size_t x, y;

    for (x = 0; x < 1000; ++x)
        for (y = 0; y < 1000; ++y)
            total += lights[x][y];

    return total;
}

int main(void)
{
    exec(stdin);

    if (ferror(stdin))
    {
        perror("Cannot read input");
        return EXIT_FAILURE;
    }

    printf("Total brightness is %ld.", measure_brightness());

    return EXIT_SUCCESS;
}
