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

int check_row(FILE* spreadsheet)
{
	int max = INT_MIN, min = INT_MAX;

	do
	{
		int cell = next_cell(spreadsheet);
		if (cell > max)
			max = cell;
		if (cell < min)
			min = cell;
	} while (!check_end_row(spreadsheet));

	return max - min;
}

int check_spreadsheet(FILE* spreadsheet)
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