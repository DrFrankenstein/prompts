#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

struct Policy
{
	unsigned low, high;
	char letter;
};

void parse_policy(FILE* stream, struct Policy* policy)
{
	int status = fscanf(stream, "%u-%u %c: ", &policy->low, &policy->high, &policy->letter);
	if (status < 3)
	{
		fputs("parse error in policy\n", stderr);
		exit(EXIT_FAILURE);
	}
}

void parse_password(FILE* stream, char* password)
{
	int status = fscanf(stream, "%25s ", password);
	if (status < 1)
	{
		fputs("parse error in password\n", stderr);
		exit(EXIT_FAILURE);
	}
}

bool check_password(char* password, struct Policy* policy)
{
	unsigned count = 0;
	while (*password)
		if (*password++ == policy->letter)
			++count;

	return count >= policy->low && count <= policy->high;
}

unsigned check_passwords(FILE* input)
{
	unsigned goodcount = 0;
	while (!feof(input))
	{
		struct Policy policy;
		parse_policy(input, &policy);

		char password[26];
		parse_password(input, password);

		if (check_password(password, &policy))
			++goodcount;
	}

	return goodcount;
}

int main(void)
{
	unsigned goodcount = check_passwords(stdin);

	printf("There are %u valid passwords.\n", goodcount);

	return EXIT_SUCCESS;
}
