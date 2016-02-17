#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int append_pair(char* dest, unsigned short count, char c)
{
    return sprintf(dest, "%hu%c", count, c);
}

void generate_string(char* dest, const char* source)
{
    char c, 
         prev = *source;
    unsigned short count = 0; /* not a char because VS does not support %hhu */
    while ((c = *source++))
    {
        if (c == prev)
            ++count;
        else
        {
            dest += append_pair(dest, count, prev);
            count = 1;
            prev = c;
        }
    }

    append_pair(dest, count, prev);
}

int main(void)
{
    const unsigned MAX_ITER = 40;
    unsigned i;
    const char initial[] = "1321131112";
    const char* prev;
    char* next;

    for (i = 0, prev = initial;
         i < MAX_ITER;
         ++i, prev = next)
    {
        /*puts(prev);*/

        next = malloc(strlen(prev) * sizeof(char) * 2 + 1);
        generate_string(next, prev);

        if (prev != initial && i < MAX_ITER - 1)
            free((void*) prev);
    }

    printf("Length of result: %u\n", strlen(prev));

    return EXIT_SUCCESS;
}