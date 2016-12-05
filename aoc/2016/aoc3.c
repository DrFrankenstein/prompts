#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Triangle { unsigned a, b, c; };

static bool triangle_valid(struct Triangle* t)
{
  return t->a < t->b + t->c
      && t->b < t->a + t->c
      && t->c < t->a + t->b;
}

static bool read_triangle(struct Triangle* triangle)
{
  int status = scanf("%u %u %u", &triangle->a, &triangle->b, &triangle->c);
  
  if (status == EOF)
    return false;
  
  if (status < 3)
  {
    perror("parse error in triangle");
    abort();
  }
  
  return true;
}

static unsigned count_triangles(void)
{
  unsigned count = 0;
  struct Triangle triangle;
  while(read_triangle(&triangle))
  {
    if (triangle_valid(&triangle))
      ++count;
  }
  
  return count;
}

int main(void)
{
  printf("There are %u valid triangles on the wall.\n", count_triangles());
  
  return EXIT_SUCCESS;
}
