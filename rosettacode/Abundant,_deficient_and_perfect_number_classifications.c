#include <stdlib.h>
#include <math.h>
#include <stdio.h>

long long sum_divisors(int n)
{
    int i;
    long long sum = 1;
    double limit;
#if 0
    int ilimit;
#endif

    if (n == 1) return 0;

    limit = sqrt((double) n);
    for (i = 2; i <= limit; i++)
        if (n % i == 0)
            sum += ((double) i == limit)? i : i + n / i;

#if 0
    for (i = 2, ilimit = n / 2; i < ilimit; i++)
    {
        div_t d = div(n, i);
        if (d.rem == 0)
        {
            sum += (i == ilimit)? i : i + d.quot;
            ilimit = d.quot;
        }
    }
#endif

    return sum;
}

const int search_limit = 20000;

int main(void)
{
    int deficient_count = 0,
        perfect_count = 0,
        abundant_count = 0,
        n;

    for (n = 1; n <= search_limit; n++)
    {
        long long pn = sum_divisors(n);
        if (pn > INT_MAX) abundant_count++; /* 'n' is an int, so any 'pn' higher than INT_MAX is clearly abundant */
        else if (pn < n) deficient_count++;
        else if (pn == n) perfect_count++;
        else /* pn > n */ abundant_count++;
    }

    printf("Deficient integers: %d\n"
           "Perfect integers: %d\n"
           "Abundant integers: %d\n",
           deficient_count, perfect_count, abundant_count);

    return EXIT_SUCCESS;
}
