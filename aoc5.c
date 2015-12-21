#pragma warning(disable:4820 4514 4710 4350 4706)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int string_nice2(const char* string)
{
    const char* ptr = string;
    int8_t repeat_pair = 0, split_repeat = 0;
    union
    {
        struct { char prev, curr, null; } c;
        char as_str[3];
    } chars;
    char third = '\0';
    chars.c.prev = *ptr++;
    chars.c.null = '\0';

    while (!(repeat_pair && split_repeat) && (chars.c.curr = *ptr++))
    {
        repeat_pair = repeat_pair || (chars.c.prev != '\0' && strstr(ptr, chars.as_str) != NULL);
        split_repeat = split_repeat || (third != '\0' && third == chars.c.curr);

        third = chars.c.prev;
        chars.c.prev = chars.c.curr;
    }

    return repeat_pair && split_repeat;
}

int string_nice1(const char* string)
{
    size_t vowel_count = 0;
    int8_t double_letter = 0;
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
    unsigned nice1 = 0, nice2 = 0;

    while (fgets(string, 18, stdin))
    {
        if (string_nice1(string))
            ++nice1;
        if (string_nice2(string))
            ++nice2;
    }

    if (ferror(stdin))
    {
        perror("Cannot read input");
        return EXIT_FAILURE;
    }

    printf("There are %u nice strings based on the first set of rules.\n", nice1);
    printf("There are %u nice strings based on the second set of rules.\n", nice2);

    return EXIT_SUCCESS;
}
