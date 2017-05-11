#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static int find_pivot(int n)
{ // simply caches the square root of a number to
  // avoid recomputing it in a loop
  static int last_n = 0;
  static int pivot = 0;

  if (n != last_n)
  {
    last_n = n;
    double dpivot = sqrt(n);
    pivot = (int) dpivot;
  }

  return pivot;
}

static int next_divisor(int n, int last)
{
  if (n < 2)
    return 0;

  int pivot = find_pivot(n);
  while (++last <= pivot)
  {
    int rem = n % last;
    if (rem == 0)
      return last;
  }

  last = n / (last - 1);

  while (--last > 1)
  {
    div_t result = div(n, last);
    if (result.rem == 0)
      return result.quot;
  }

  return 0;
}

static unsigned count_divisors(int n)
{
  unsigned count = 0;
  int pivot = find_pivot(n);

  int divisor = 0;
  while ((divisor = next_divisor(n, divisor)) && divisor <= pivot)
  {
    if (divisor == 1 || divisor == pivot)
      ++count;
    else
      count += 2;
  }

  return count;
}

static void print_divisors(int ubound)
{
  for (int n = 1; n <= ubound; ++n)
  {
    printf("Divisors of %d: ", n);
    int divisor = 0;
    while ((divisor = next_divisor(n, divisor)))
      printf("%d ", divisor);

    putchar('\n');
  }
}

static void find_most_divisors(int ubound)
{
  int most_n = 0;
  unsigned most_count = 0;
  for (int n = 1; n <= ubound; ++n)
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

