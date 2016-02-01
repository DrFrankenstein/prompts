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
        /* fall thru */

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

size_t measure_escaped_string(const char* string)
{
    size_t len = 0;
    char c;

    while ((c = *string++) && c != '\n')
    {
        if (c == '"' || c == '\\')
            ++len;

        ++len;
    }

    return len + 2;
}

int main(void)
{
    char str[50];
    size_t parsed_diff = 0, escaped_diff = 0;

    while (fgets(str, 50, stdin))
    {
        size_t len = strlen(str) - 1; /* '- 1' because fgets includes the line return in the string */
        
        parsed_diff += len - parse_and_measure_string(str);
        escaped_diff += measure_escaped_string(str) - len;
    }

    if (ferror(stdin))
    {
        perror("Could not read input");
        return EXIT_FAILURE;
    }

    printf("The total size difference after parsing is %u.\n", parsed_diff);
    printf("The total size difference after escaping is %u.\n", escaped_diff);

    return EXIT_SUCCESS;
}
