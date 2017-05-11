#include <math.h>
#include <stdio.h>

int find_pivot(n)
{ /* simply caches the square root of a number to
     avoid recomputing it in a loop */
  static int last_n = 0;
  static int pivot = 0;

  if (n != last_n)
  {
    double dpivot = sqrt((double) n);
    pivot = (int) dpivot;
    last_n = n;
  }

  return pivot;
}

int next_divisor(n, last)
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

unsigned count_divisors(n)
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

print_divisors(ubound)
{
  int n;
  for (n = 1; n <= ubound; ++n)
  {
    int divisor = next_divisor(n, 0);
    while (divisor != 0)
    {
      printf("%d ", divisor);
      divisor = next_divisor(n, divisor);
    }

    putchar('\n');
  }
}

find_most_divisors(ubound)
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

int main()
{
  print_divisors(10);
  find_most_divisors(20000);

  return 0;
}
