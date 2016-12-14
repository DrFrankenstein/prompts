#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

struct RepeatMarker { size_t len; unsigned repeats; };

static long parse_marker(struct RepeatMarker* marker)
{
  long at = ftell(stdin);
  int status = scanf("%zux%u", &marker->len, &marker->repeats);
  
  if (status != 2 || getchar() != ')' || marker->len > INT_MAX)
  {
    fseek(stdin, at, SEEK_SET);
    return 0;
  }
  
  return ftell(stdin) - at;
}

static size_t decompressed_len(long chunklen)
{
  size_t len = 0;
  int c;
  bool has_chunklen = chunklen != -1;
  while ((!has_chunklen || chunklen > 0) && (c = getchar()) != EOF)
  {
    if (c == '(')
    {
      struct RepeatMarker marker;
      long parsed = parse_marker(&marker);
      if (parsed > 0)
      {
        len += marker.repeats * decompressed_len((int) marker.len);
        chunklen -= parsed + (long) marker.len + 1;
      }
      else
      {
        ++len;
        --chunklen;
      }
    }
    else if (!isspace(c))
    {
      ++len;
      --chunklen;
    }
  }
  
  return len;
}

int main(void)
{
  printf("The length of the decompressed file is %zu.\n", decompressed_len(-1));
}
