#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char* skip_escape(const char* source)
{
    switch (source[1])
    {
    case '\\':
    case '"':
        return source + 1;

    case 'x':
        if (isxdigit(source[2]) && isxdigit(source[3]))
            return source + 3;
        else
            return source;

    default:
        return source;
    }
}

size_t parse_and_measure_string(const char* source)
{
    char c;
    size_t len = 0;

    if (source[0] != '"')
    {
        fputs("Error: expected quotation mark at beginning of string\n", stderr);
        return 0;
    }

    while ((c = *++source) && c != '"')
    {
        if (c == '\\')
            source = skip_escape(source);

        ++len;
    }

    if (c != '"')
    {
        fputs("Error: unfinished string\n", stderr);
        return 0;
    }

    return len;
}

int main(void)
{
    char str[50];
    size_t sum = 0;

    while (fgets(str, 50, stdin))
    {   /* '- 1' because fgets includes the line return in the string */
        sum += strlen(str) - 1 - parse_and_measure_string(str);
    }

    if (ferror(stdin))
    {
        perror("Could not read input");
        return EXIT_FAILURE;
    }

    printf("The total size difference is %u.", sum);

    return EXIT_SUCCESS;
}
