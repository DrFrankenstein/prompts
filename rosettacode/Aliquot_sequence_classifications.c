#include <stdlib.h>
#include <math.h>
#include <stdio.h>

enum Aliquot
{
    TERMINATING,
    PERFECT,
    AMICABLE,
    SOCIABLE,
    ASPIRING,
    CYCLIC,
    NONTERMINATING
};

static long sum_divisors(long n)
{
    double dlimit = sqrt((double) n);
    long limit = (long) dlimit;
    long c;
    long sum = 1;

    if (n == 1)
        return 0;

    for (c = 2; c <= limit; ++c)
    {
        ldiv_t result = ldiv(n, c);
        if (result.rem == 0)
        {
            sum += c;
            if (c != result.quot)
                sum += result.quot;
        }
    }

    return sum;
}

static enum Aliquot check_class(long* terms, size_t idx)
{
    long curr = terms[idx];

    if (curr == 0)
        return TERMINATING;

    if (curr == terms[0])
    {
        if (idx == 1)
            return PERFECT;
        else if (idx == 2)
            return AMICABLE;
        else
            return SOCIABLE;
    }

    if (terms[idx - 1] == curr)
        return ASPIRING;

    for (--idx; idx > 0; --idx)
        if (terms[idx] == curr)
            return CYCLIC;
    
    return NONTERMINATING;
}

static enum Aliquot classify(long k)
{
    long terms[17];
    size_t i;
    enum Aliquot class;

    terms[0] = k;

    for (i = 1; i < 17; ++i)
    {
        if (k > 140737488355328)
            return NONTERMINATING;

        k = sum_divisors(k);
        terms[i] = k;
        printf(" %ld", k);

        class = check_class(terms, i);
        if (class != NONTERMINATING)
            return class;
    }

    return NONTERMINATING;
}

static const char* class_name(enum Aliquot class)
{
    switch (class)
    {
        case TERMINATING: return "Terminating";
        case PERFECT: return "Perfect";
        case AMICABLE: return "Amicable";
        case SOCIABLE: return "Sociable";
        case ASPIRING: return "Aspiring";
        case CYCLIC: return "Cyclic";
        case NONTERMINATING: return "Non-terminating";
    }

    abort(); /* keep gcc happy */
}

static const long ks[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 28, 496, 220, 1184, 12496, 1264460,
    790, 909, 562, 1064, 1488
};
static const size_t kcount = sizeof ks / sizeof *ks;

int main()
{
    size_t idx;
    for (idx = 0; idx < kcount; ++idx)
    {
        enum Aliquot class;
        long k = ks[idx];
        const char* name;

        printf("%ld:", k);
        class = classify(k);
        name = class_name(class);
        printf(" %s\n", name);
    }

    return EXIT_SUCCESS;
}

