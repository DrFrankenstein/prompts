#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define MAX_CITIES 10

struct route;

struct city
{
    char name[16];
    struct route* routes;
};

struct city* add_city(struct city** cities, const char* name, size_t maxcount)
{
    struct city* c;

    while (*cities && maxcount--)
        cities++;
    if (!maxcount) return NULL;

    c = *cities = calloc(1, sizeof **cities);
    if (c) strcpy(c->name, name);

    return c;
}

struct city* find_city(struct city** cities, const char* name)
{
    while (*cities)
    {
        if (strcmp((*cities)->name, name) == 0)
            return *cities;

        ++cities;
    }

    return NULL;
}

struct city* find_or_add_city(struct city** cities, const char* name, size_t maxcount)
{
    struct city* c;
    if ((c = find_city(cities, name)) == NULL)
        c = add_city(cities, name, maxcount);

    return c;
}

struct route
{
    struct city* to;
    unsigned distance;

    struct route* next;
};

void do_add_route(struct city* from, struct city* to, unsigned distance)
{
    struct route* rte = from->routes,
                * newrte = calloc(1, sizeof *newrte);

    if (!newrte)
    {
        perror("can't create route");
        exit(EXIT_FAILURE);
    }

    newrte->to = to;
    newrte->distance = distance;

    if (!rte)
        from->routes = newrte;
    else
    {
        while (rte->next)
            rte = rte->next;

        rte->next = newrte;
    }
}

void add_route(struct city* from, struct city* to, unsigned distance)
{
    do_add_route(from, to, distance);
    do_add_route(to, from, distance);
}

int parse_input(FILE* stream, struct city** cities, size_t maxcount)
{
    size_t i = 0;
    while (i < maxcount && !feof(stream))
    {
        char from[16], to[16];
        unsigned distance;
        struct city* fromcity, 
                   * tocity;
        int parse_status = scanf("%15s to %15s = %u\n", from, to, &distance);

        if (parse_status != 3)
        {
            char garbage[21];

            if (ferror(stream))
                perror("can't read input");
            else
            {
                fgets(garbage, 20, stream);
                fprintf(stderr, "parse error near '%s'\n", garbage);
            }

            return 0;
        }

        fromcity = find_or_add_city(cities, from, maxcount);
        if (!fromcity)
        {
            fprintf(stderr, "could not add city '%s'\n", from);
            return 0;
        }

        tocity = find_or_add_city(cities, to, maxcount);
        if (!tocity) 
        {
            fprintf(stderr, "could not add city '%s'\n", to);
            return 0;
        }

        add_route(fromcity, tocity, distance);
    }

    return 1;
}

unsigned distance_to(struct city* c, const char* name)
{
    struct route* rte = c->routes;

    while (rte)
    {
        if (strcmp(rte->to->name, name) == 0)
            return rte->distance;

        rte = rte->next;
    }

    return 0;
}

unsigned measure_route(struct city** cities)
{
    unsigned distance = 0;
    while (cities[1])
    {
        unsigned leg = distance_to(cities[0], cities[1]->name);
        if (!leg)
            /* hit a dead end */
            return 0;

        distance += leg;
        ++cities;
    }

    return distance;
}

void print_route(struct city** cities)
{
    while (*cities)
    {
        struct city* c = *cities;
        printf("%s ", c->name);

        if (*++cities)
            fputs("-> ", stdout);
    }
}

void swap_cities(struct city** left, struct city** right)
{
    struct city* mid = *left;
    *left = *right;
    *right = mid;
}

void try_route(struct city** cities, unsigned* shortest, unsigned* longest)
{
    unsigned distance = measure_route(cities);

    if (!distance)
        /* can't get here from there */
        return;

    /*print_route(cities);
    printf("= %u", distance);*/

    if (distance < *shortest)
    {
        *shortest = distance;

        fputs("New shortest: ", stdout);
        print_route(cities);
        printf("= %u\n", distance);
        /*putchar('*');*/
    }

    if (distance > *longest)
    {
        *longest = distance;

        fputs("New longest: ", stdout);
        print_route(cities);
        printf("= %u\n", distance);
    }

    /*putchar('\n');*/
}

void try_routes(struct city** cities, size_t count, unsigned* shortest, unsigned* longest)
{
    size_t i;

    if (count == 1)
        try_route(cities, shortest, longest);
    else
    {
        for (i = 0; i < count - 1; ++i)
        {
            try_routes(cities, count - 1, shortest, longest);

            if ((count & 1) == 0)
                swap_cities(cities + i, cities + (count - 1));
            else
                swap_cities(cities, cities + (count - 1));
        }

        try_routes(cities, count - 1, shortest, longest);
    }
}

int main(void)
{
    struct city* cities[MAX_CITIES + 1] = {0};
    unsigned shortest = UINT_MAX, longest = 0;
    size_t count = 0;

    if (!parse_input(stdin, cities, MAX_CITIES))
        return EXIT_FAILURE;

    while (cities[count])
        ++count;

    try_routes(cities, count, &shortest, &longest);
    
    printf("The shortest distance is %u\n"
           "The longest distance is %u\n", 
           shortest, longest);

    return EXIT_SUCCESS;
}
