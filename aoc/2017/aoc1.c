#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int char_to_digit(int c)
{
	return c - '0';
}

int get_next_digit(FILE* stream)
{
	int c;
	do
	{
		c = getchar();
	} while (!isdigit(c) && !feof(stream));

	return c;
}

int solve_reverse_captcha(FILE* stream)
{
	int first = getchar(),
		c1 = 0, 
		c2 = first,
		sum = 0;

	while (!feof(stdin))
	{
		c1 = c2;

		c2 = get_next_digit(stdin);

		if (c1 == c2)
			sum += char_to_digit(c1);
	}

	if (c1 == first)
		sum += char_to_digit(c1);
}

int main(void)
{
	int sum = solve_reverse_captcha(stdin);
	printf("Sum: %d\n", sum);

	return EXIT_SUCCESS;
}