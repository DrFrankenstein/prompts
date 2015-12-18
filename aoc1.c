#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    int ch, floor = 0;
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
    }

    if (ferror(stdin))
    {
        perror("Cannot read input");
        return EXIT_FAILURE;
    }

    printf("Floor: %d\n", floor);

    return EXIT_SUCCESS;
}
