#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static unsigned char char_to_digit(char c)
{
	return (unsigned char) (c - '0');
}

static char* read_file(FILE* stream, size_t* len)
{
	char* string = NULL;
	size_t size = 0;
	while (!feof(stream))
	{
#pragma warning(suppress: 6308) // we're bailing out if it fails anyway
		string = realloc(string, size + 1);

		if (string == NULL)
			exit(EXIT_FAILURE);

		int ch = getc(stream);
		if (ch == EOF)
			ch = '\0';
		else if (!isdigit(ch))
			continue;

		string[size++] = (char) ch;
	}

	*len = size - 1;
	return string;
}

static unsigned solve_reverse_captcha(FILE* stream)
{
	unsigned sum = 0;
	size_t len;
	char* string = read_file(stream, &len);

	size_t middle = len / 2;

	for (size_t p1 = 0, p2 = middle; p1 < middle; p1++, p2++)
	{
		if (string[p1] == string[p2])
			sum += char_to_digit(string[p1]);
	}

	return sum * 2;
}

int main(void)
{
	unsigned sum = solve_reverse_captcha(stdin);
	printf("Sum: %u\n", sum);

	return EXIT_SUCCESS;
}
