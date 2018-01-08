#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

typedef char Word[16];
struct WordEntry
{
	Word word;
	struct WordEntry* next;
};
enum { DICT_SIZE = 8 };
typedef struct WordEntry* Dict[DICT_SIZE];

static uint32_t hash_word(Word word)
{
	uint32_t hash = 0;
	char* p = word;
	while (*p)
	{
		hash = ((hash << 8) | (hash >> 24)) ^ *p;
		++p;
	}

	return hash;
}

static bool check_word(Word word, Dict dict)
{
	uint32_t idx = hash_word(word) % DICT_SIZE;
	struct WordEntry* ent = dict[idx];
	struct WordEntry* prev = NULL;

	while (ent)
	{
		if (strcmp(ent->word, word) == 0)
			return false;

		prev = ent;
		ent = ent->next;
	}

	ent = malloc(sizeof *ent);

	if (!ent)
	{
		perror("can't create dict nodes");
		exit(EXIT_FAILURE);
	}

	strcpy(ent->word, word);
	ent->next = NULL;

	if (prev)
		prev->next = ent;
	else
		dict[idx] = ent;

	return true;
}

static void clear_dict(Dict dict)
{
	for (size_t idx = 0; idx < DICT_SIZE; ++idx)
	{
		struct WordEntry* ent = dict[idx];

		while (ent)
		{
			struct WordEntry* next = ent->next;
			free(ent);
			ent = next;
		}
	}
}

static bool check_end_phrase(FILE* list)
{
	int c;
	do
	{
		c = fgetc(list);
		if (c == '\n' || c == EOF)
			return true;
	} while (isspace(c));

	ungetc(c, list);

	return false;
}


static bool next_word(FILE* list, Word word)
{
	int read = fscanf(list, "%15s", word);
	if (read != 1)
	{
		if (feof(list) && !ferror(list))
		{	// read empty line
			word[0] = '\0';
			return true;
		}

		perror("couldn't read passphrase list");
		exit(EXIT_FAILURE);
	}

	return check_end_phrase(list);
}

static void purge_line(FILE* list)
{
	if (feof(list))
		return;

	int c;
	do
	{
		c = getc(list);
	} while (c != '\n');
}

static bool validate_next_passphrase(FILE* list)
{
	Dict dict = { 0 };
	Word word;
	
	bool last = false;
	while (!last)
	{
		last = next_word(list, word);

		if (!word[0] || !check_word(word, dict))
		{
			clear_dict(dict);
			if (!last)
				purge_line(list);
			return false;
		}
	}

	clear_dict(dict);
	return true;
}

static unsigned validate_passphrase_list(FILE* list)
{
	unsigned count = 0;
	while (!feof(list))
		if (validate_next_passphrase(list))
			++count;

	return count;
}

int main(void)
{
	printf("%u passphrases are valid.\n", validate_passphrase_list(stdin));
}
