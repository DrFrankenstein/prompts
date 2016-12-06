#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// CL doesn't support static array arguments
#ifdef _MSC_VER
# define STATIC_ARRAY_SIZE
#else
# define STATIC_ARRAY_SIZE static
#endif

static bool parse_name(unsigned counts[STATIC_ARRAY_SIZE 26])
{
  int c;
  while ((c = getchar()) != EOF)
  {
    if (isalpha(c))
      ++counts[c - 'a'];
    else if (isdigit(c))
    {
      ungetc(c, stdin);
      break;
    }
    else if (c != '-')
    {
      fprintf(stderr, "error: stray character '%c' in input\n", c);
      abort();
    }
  }
  
  return c != EOF;
}

static size_t top_letter_index(unsigned counts[STATIC_ARRAY_SIZE 26])
{
  size_t top_idx = 0;
  unsigned top_count = 0;
  for (size_t idx = 0; idx < 26; ++idx)
  {
    unsigned count = counts[idx];
    if (count > top_count)
    {
      top_idx = idx;
      top_count = count;
    }
  }
  
  return top_idx;
}

static void top_five_letters(unsigned counts[STATIC_ARRAY_SIZE 26], char top[STATIC_ARRAY_SIZE 5])
{
  for (size_t idx = 0; idx < 5; ++idx)
  {
    size_t top_idx = top_letter_index(counts);
    counts[top_idx] = 0;
    top[idx] = (char)('a' + top_idx);
  }
}

static unsigned next_room_sector(void)
{
  unsigned counts[26] = { 0 };
  if (!parse_name(counts))
    return 0;
  
  char top[5];
  top_five_letters(counts, top);

  unsigned sector;
  char chk[5];
  int status = scanf("%u[%5c] ", &sector, chk);
  
  if (status < 2)
  {
    perror("error: parse error in input; stream status");
    abort();
  }
  
  if (memcmp(top, chk, 5) != 0)
    return 0;
  
  return sector;
}

int main(void)
{
  unsigned sum = 0;
  while (!feof(stdin))
  {
    sum += next_room_sector();
  }
  
  printf("The sum of all the sector IDs is %u.\n", sum);
  
  return EXIT_SUCCESS;
}
