#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char parse_escape(const char** source)
{
    const char* pos = *source;
    char c = *++pos;

    switch (c)
    {
    case '\\':
    case '"':
        *source = pos;
        break;

    case 'x':
        {
        short s;    /* VC++ doesn't support format size prefix "hh" for 'char' */
        if (!sscanf(++pos, "%2hx", &s))
            return '\0';
        c = (char) s;
        *source = pos + 1;
        break;
        }

    default:
        return '\0';
    }

    return c;
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
            (void) parse_escape(&source);

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
