#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

struct RepeatMarker { size_t len; unsigned repeats; };

static bool parse_marker(struct RepeatMarker* marker)
{
  long at = ftell(stdin);
  int status = scanf("%zux%u", &marker->len, &marker->repeats);
  
  if (status != 2 || getchar() != ')')
  {
    fseek(stdin, at, SEEK_SET);
    return false;
  }
  
  return true;
}

static void skip(size_t len)
{
  int c;
  while (len != 0 && (c = getchar()) != EOF)
    if (!isspace(c))
      --len;
}

static size_t decompressed_len(void)
{
  size_t len = 0;
  int c;
  while ((c = getchar()) != EOF)
  {
    if (c == '(')
    {
      struct RepeatMarker marker;
      if (parse_marker(&marker))
      {
        len += marker.len * marker.repeats;
        skip(marker.len);
      }
      else
        ++len;
    }
    else if (!isspace(c))
      ++len;
  }
  
  return len;
}

int main(void)
{
  printf("The length of the decompressed file is %zu.\n", decompressed_len());
}
