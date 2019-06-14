#include <stdlib.h>
#include <stdio.h>

typedef unsigned char uint8_t;

void print_doors(uint8_t doors[])
{
    uint8_t door;
    for (door = 0; door < 100; door++)
        printf("Door #%u: %s\n", (unsigned) door + 1, doors[door]? "open" : "closed");
}

void doors(void)
{
    uint8_t doors[100] = {0},
        pass, door;
    for (pass = 1; pass <= 100; pass++)
        for (door = 0; door < 100; door += pass)
            doors[door] = !doors[door];

    print_doors(doors);
}

void doors_opt(void)
{
    uint8_t doors[100] = {0},
        factor, door = 0;
    for(factor = 1; (door += (factor * 2) - 1) < 100; factor++)
        doors[door] = !doors[door];

    print_doors(doors);
}

int main(void)
{
    doors();
    doors_opt();

    return EXIT_SUCCESS;
}
