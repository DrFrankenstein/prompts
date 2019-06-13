#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    int ch, floor = 0, been_basement = 0;
    while ((ch = getchar()) != EOF)
    {
        switch (ch)
        {
        case '(':   ++floor;    break;
        case ')':   --floor;    break;
        default:
            printf("Warning: unexpected character in input: %c\n", ch);
            break;
        }

        if (!been_basement && floor == -1)
        {
            printf("Note: first entered basement at step %ld\n", ftell(stdin));
            been_basement = 1;
        }
    }

    if (ferror(stdin))
    {
        perror("Cannot read input");
        return EXIT_FAILURE;
    }

    printf("Floor: %d\n", floor);

    return EXIT_SUCCESS;
}
