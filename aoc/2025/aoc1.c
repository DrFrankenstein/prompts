#include <stdlib.h>
#include <stdio.h>

struct Command
{
		char direction;
		int count;
};

static struct Command parse_command(FILE* input)
{
		struct Command command;
		const int parse_result = fscanf(input, "%c%d ", &command.direction, &command.count);
		if (parse_result != 2)
		{
				fputs("parse error\n", stderr);
				exit(EXIT_FAILURE);
		}

		return command;
}

static int normalize_dial_position(int position)
{
		position %= 100;
		if (position < 0)
		{
				position += 100;
		}

		return position;
}

static int exec_command(int position, const struct Command command)
{
		switch (command.direction)
		{
				case 'L':
						position -= command.count;
						break;
				case 'R':
						position += command.count;
						break;
				default:
						fputs("invalid turn direction", stderr);
						exit(EXIT_FAILURE);
		}

		return normalize_dial_position(position);
}

int main(void)
{
		int position = 50;
		unsigned zerocount = 0;

		while (!feof(stdin))
		{
				struct Command command = parse_command(stdin);
				position = exec_command(position, command);

				if (position == 0)
				{
						++zerocount;
				}
		}

		printf("Password = %u\n", zerocount);
}
