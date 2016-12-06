#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// CL doesn't support static array arguments
#ifdef _MSC_VER
# define STATIC_ARRAY_SIZE
#else
# define STATIC_ARRAY_SIZE static
#endif

struct Triangle { unsigned a, b, c; };

static bool triangle_valid(struct Triangle* t)
{
  return t->a < t->b + t->c
      && t->b < t->a + t->c
      && t->c < t->a + t->b;
}

static bool read_triangles(struct Triangle triangles[STATIC_ARRAY_SIZE 3])
{
  int status = scanf("%u %u %u %u %u %u %u %u %u",
      &triangles[0].a, &triangles[1].a, &triangles[2].a,
      &triangles[0].b, &triangles[1].b, &triangles[2].b,
      &triangles[0].c, &triangles[1].c, &triangles[2].c);
  
  if (status == EOF)
    return false;
  
  if (status < 9)
  {
    perror("parse error in triangles");
    abort();
  }
  
  return true;
}

static unsigned count_triangles(void)
{
  unsigned count = 0;
  struct Triangle triangles[3];
  while(read_triangles(triangles))
  {
    if (triangle_valid(&triangles[0]))
      ++count;
    if (triangle_valid(&triangles[1]))
      ++count;
    if (triangle_valid(&triangles[2]))
      ++count;
  }
  
  return count;
}

int main(void)
{
  printf("There are %u valid triangles on the wall.\n", count_triangles());
  
  return EXIT_SUCCESS;
}
