#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static int find_pivot(int n)
{ /* simply caches the square root of a number to
     avoid recomputing it in a loop */
  static int last_n = 0;
  static int pivot = 0;

  if (n != last_n)
  {
    double dpivot = sqrt(n);
    pivot = (int) dpivot;
    last_n = n;
  }

  return pivot;
}

static int next_divisor(int n, int last)
{
  int pivot;

  if (n < 2)
    return 0;

  pivot = find_pivot(n);
  while (++last <= pivot)
  {
    int rem = n % last;
    if (rem == 0)
      return last;
  }

  last = n / (last - 1);

  while (--last > 1)
  {
    if (n % last == 0)
      return n / last;
  }

  return 0;
}

static unsigned count_divisors(int n)
{
  unsigned count = 0;
  int pivot = find_pivot(n);

  int divisor = next_divisor(n, 0);
  while (divisor != 0 && divisor <= pivot)
  {
    if (divisor == 1 || divisor == pivot)
      ++count;
    else
      count += 2;
    
    divisor = next_divisor(n, divisor);
  }

  return count;
}

static void print_divisors(int ubound)
{
  int n;
  for (n = 1; n <= ubound; ++n)
  {
    int divisor = next_divisor(n, 0);
    printf("Divisors of %d: ", n);
    while (divisor != 0)
    {
      printf("%d ", divisor);
      divisor = next_divisor(n, divisor);
    }

    putchar('\n');
  }
}

static void find_most_divisors(int ubound)
{
  int most_n = 0;
  unsigned most_count = 0;
  int n;
  for (n = 1; n <= ubound; ++n)
  {
    unsigned count = count_divisors(n);
    if (count > most_count)
    {
      most_count = count;
      most_n = n;
    }
  }

  printf("Most divisors: %d: %u divisors\n",
    most_n, most_count);
}

int main(void)
{
  print_divisors(10);
  find_most_divisors(20000);

  return EXIT_SUCCESS;
}
