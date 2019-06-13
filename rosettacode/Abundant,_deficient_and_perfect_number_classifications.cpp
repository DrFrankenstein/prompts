#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

#include <boost/range/irange.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/numeric.hpp>

using namespace std;
using namespace boost;
using namespace boost::adaptors;

int sum_divisors(int n)
{
    if (n == 1) return 0;

    double limit = sqrt(n);
    if (limit < 2.) return 1;

    return accumulate(
        irange(2, (int) limit)
        | filtered([&](int i) { return n % i == 0; })
        | transformed([&](int i) { return ((double) i) == limit? i : i + n / i; }),

        1
    );
}

const int search_limit = 20000;

int main()
{
    int deficient_count = 0,
        perfect_count = 0,
        abundant_count = 0;

    for (int n : irange(1, search_limit))
    {
        int pn = sum_divisors(n);
        if (pn < n) deficient_count++;
        else if (pn == n) perfect_count++;
        else /* pn > n */ abundant_count++;
    }

    cout << "Deficient integers: " << deficient_count << endl
         << "Perfect integers: " << perfect_count << endl
         << "Abundant integers: " << abundant_count << endl
    ;

    return EXIT_SUCCESS;
}
