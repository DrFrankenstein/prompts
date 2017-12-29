#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

static bool check_end_row(FILE* spreadsheet)
{
	int c;
	do
	{
		c = fgetc(spreadsheet);
		if (c == '\n' || c == EOF)
			return true;
	} while (isspace(c));

	ungetc(c, spreadsheet);

	return false;
}

static int next_cell(FILE* spreadsheet)
{
	int cell;
	int status = fscanf(spreadsheet, "%d", &cell);

	if (status < 1)
		return 0;

	return cell;
}

static bool try_divide(int first, int second, int* result)
{
	if (first < second)
	{
		int t = first;
		first = second;
		second = t;
	}

	if (first % second == 0)
	{
		*result = first / second;
		return true;
	}

	return false;
}

static size_t read_row(FILE* spreadsheet, int row[])
{
	size_t count = 0;

	while (!check_end_row(spreadsheet))
		row[count++] = next_cell(spreadsheet);

	return count;
}

static int do_check_row(const int row[], const size_t count)
{
	for (size_t first = 0; first < count - 1; ++first)
		for (size_t second = first + 1; second < count; ++second)
		{
			int result;
			if (try_divide(row[first], row[second], &result))
				return result;
		}

	return -1;
}

static int check_row(FILE* spreadsheet)
{
	int row[20];
	size_t count = 0;

	count = read_row(spreadsheet, row);

	if (count == 0)
		return 0; // empty line

	return do_check_row(row, count);
}

static int check_spreadsheet(FILE* spreadsheet)
{
	int checksum = 0;
	while (!feof(spreadsheet))
		checksum += check_row(spreadsheet);

	return checksum;
}

int main(void)
{
	printf("Checksum: %d\n", check_spreadsheet(stdin));
}
