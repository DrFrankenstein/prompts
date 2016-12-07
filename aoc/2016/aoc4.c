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

static void classify_letters(const char* string, unsigned char counts[STATIC_ARRAY_SIZE 26])
{
  int c;
  while ((c = (int) *string++))
    if (isalpha(c))
      ++counts[c - 'a'];
}

static size_t top_letter_index(unsigned char counts[STATIC_ARRAY_SIZE 26])
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

static void top_five_letters(const char* string, char top[STATIC_ARRAY_SIZE 5])
{
  unsigned char counts[26] = { 0 };
  classify_letters(string, counts);
  
  for (size_t idx = 0; idx < 5; ++idx)
  {
    size_t top_idx = top_letter_index(counts);
    counts[top_idx] = 0;
    top[idx] = (char)('a' + top_idx);
  }
}

static void ceasar_decrypt(char* text, unsigned key)
{
  char value = (char) (key % 26);
  
  char c;
  while ((c = *text))
  {
    if (c == '-')
      *text = ' ';
    else
      *text = (char) ((c - 'a' + value) % 26 + 'a');
    
    ++text;
  }
}

static unsigned process_next_room(void)
{
  char name[101];
  unsigned sector;
  char chk[5];
  // read up to 50 letters or dashes, then an unsigned number,
  // then 5 characters between brackets, then whitespace (line break)
  int status = scanf(
    "%100[abcdefghijklmnopqrstuvwxyz-]%u[%5c] ",
    name, &sector, chk
  );
  
  if (status == EOF)
    return 0;
  
  if (status < 3)
  {
    perror("error: parse error in input; stream status");
    abort();
  }
  
  char top[5];
  top_five_letters(name, top);
  
  if (memcmp(top, chk, 5) != 0)
    return 0;
  
  ceasar_decrypt(name, sector);
  if (strstr(name, "northpole object"))
    printf("Sector: %3u, Room: %s\n", sector, name);
  
  return sector;
}

int main(void)
{
  unsigned sum = 0;
  while (!feof(stdin))
    sum += process_next_room();
  
  printf("The sum of all the sector IDs is %u.\n", sum);
  
  return EXIT_SUCCESS;
}
