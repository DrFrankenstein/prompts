#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Policy
{
	unsigned low, high;
	char letter;
};

struct Results
{
	unsigned rule1, rule2;
};

static void parse_policy(FILE* stream, struct Policy* policy)
{
	int status = fscanf(stream, "%u-%u %c: ", &policy->low, &policy->high, &policy->letter);
	if (status < 3)
	{
		fputs("parse error in policy\n", stderr);
		exit(EXIT_FAILURE);
	}
}

static void parse_password(FILE* stream, char* password)
{
	int status = fscanf(stream, "%25s ", password);
	if (status < 1)
	{
		fputs("parse error in password\n", stderr);
		exit(EXIT_FAILURE);
	}
}

static bool check_password1(char* password, struct Policy* policy)
{
	unsigned count = 0;
	while (*password)
		if (*password++ == policy->letter)
			++count;

	return count >= policy->low && count <= policy->high;
}

static bool check_password2(char* password, struct Policy* policy)
{
	assert(policy->low <= strlen(password) && policy->high <= strlen(password));

	return (password[policy->low - 1] == policy->letter)
	    != (password[policy->high - 1] == policy->letter);
}

static struct Results check_passwords(FILE* input)
{
	struct Results results = { 0 };
	while (!feof(input))
	{
		struct Policy policy;
		parse_policy(input, &policy);

		char password[26];
		parse_password(input, password);

		if (check_password1(password, &policy))
			++results.rule1;
		if (check_password2(password, &policy))
			++results.rule2;
	}

	return results;
}

int main(void)
{
	struct Results results = check_passwords(stdin);

	printf("There are %u valid passwords under the first rule.\n"
	       "There are %u valid passwords under the second rule.\n",
		results.rule1, results.rule2);

	return EXIT_SUCCESS;
}
