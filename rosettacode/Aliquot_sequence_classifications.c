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

static long long sum_divisors(long long n)
{
    if (n == 1)
        return 0;

    long long sum = 1;
    double dlimit = sqrt((double) n);
    long long limit = (long long) dlimit;

    for (long long c = 2; c <= limit; ++c)
    {
        lldiv_t result = lldiv(n, c);
        if (result.rem == 0)
        {
            sum += c;
            if (c != result.quot)
                sum += result.quot;
        }
    }

    return sum;
}

static enum Aliquot check_class(long long* terms, size_t idx)
{
    long long curr = terms[idx];

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

static enum Aliquot classify(long long k)
{
    long long terms[17];
    terms[0] = k;
    for (size_t i = 1; i < 17; ++i)
    {
        if (k > 140737488355328)
            return NONTERMINATING;

        k = sum_divisors(k);
        terms[i] = k;
        printf(" %lld", k);

        enum Aliquot class = check_class(terms, i);
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

    abort(); // keep gcc happy
}

static const long long ks[] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 28, 496, 220, 1184, 12496, 1264460,
    790, 909, 562, 1064, 1488, 15355717786080
};
static const size_t kcount = sizeof ks / sizeof *ks;

int main()
{
    for (size_t idx = 0; idx < kcount; ++idx)
    {
        long long k = ks[idx];
        printf("%lld:", k);
        enum Aliquot class = classify(k);
        const char* name = class_name(class);
        printf(" %s\n", name);
    }
}
