#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct Command
{
	char direction;
	int count;
};

static void debugf(const char* fmt, ...)
{
#ifdef _DEBUG
	va_list vargs;
	va_start(vargs, fmt);
	vfprintf(stderr, fmt, vargs);
	va_end(vargs);
#endif
}

static struct Command parse_command(FILE* input)
{
	struct Command command;
	const int parse_result = fscanf(input, "%c%d ", &command.direction, &command.count);
	if (parse_result != 2)
	{
		fputs("parse error\n", stderr);
		exit(EXIT_FAILURE);
	}

	debugf("%c%3d:\t", command.direction, command.count);

	return command;
}

struct DialState
{
	int position;
	int zeropasses;
};

static struct DialState normalize_dial_position(const int position, const bool fromzero)
{
	int zeropasses = abs(position / 100);
	int newposition = position % 100;

	if (position <= 0 && !fromzero)
	{
		++zeropasses;
	}

	if (newposition < 0)
	{
		newposition += 100;
	}

	debugf("position = (% 4d) %2d\t zeropasses = %2d\t ", position, newposition, zeropasses);

	return (struct DialState){ newposition, zeropasses };
}

static struct DialState exec_command(const int position, const struct Command command)
{
	int newposition;
	switch (command.direction)
	{
		case 'L':
			newposition = position - command.count;
			break;
		case 'R':
			newposition = position + command.count;
			break;
		default:
			fputs("invalid turn direction", stderr);
			exit(EXIT_FAILURE);
	}

	if (command.count == 0)
	{
		return (struct DialState){ position, 0 };
	}

	return normalize_dial_position(newposition, position == 0);
}

int main(void)
{
	int position = 50;
	int zerocount = 0;
	int zeropasses = 0;

	while (!feof(stdin))
	{
		const struct Command command    = parse_command(stdin);
		const struct DialState newstate = exec_command(position, command);

		position = newstate.position;
		zeropasses += newstate.zeropasses;

		if (position == 0)
		{
			++zerocount;
		}

		debugf("pw1 = %5d\t pw2 = %5d\n", zerocount, zeropasses);
	}

	printf(
		"Password = %d\n"
		"Password (0x434C49434B) = %d\n",
		zerocount, zeropasses);
}
