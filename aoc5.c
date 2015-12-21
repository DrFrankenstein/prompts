#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int string_nice(const char* string)
{
    size_t vowel_count = 0;
    int double_letter = 0;
    const char* ptr = string;
    char c, prev = '\0';

    while ((c = *ptr++))
    {
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
            ++vowel_count;

        if (c == prev) double_letter = 1;

        if (prev == 'a' && c == 'b'
         || prev == 'c' && c == 'd'
         || prev == 'p' && c == 'q'
         || prev == 'x' && c == 'y')
        {
            return 0;
        }

        prev = c;
    }

    return double_letter && vowel_count >=3;
}

int main(void)
{
    char string[18];
    unsigned nice = 0;

    while (fgets(string, 18, stdin))
    {
        if (string_nice(string))
            ++nice;
    }

    if (ferror(stdin))
    {
        perror("Cannot read input");
        return EXIT_FAILURE;
    }

    printf("There are %u nice strings.\n", nice);

    return EXIT_SUCCESS;
}
