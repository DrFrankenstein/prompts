#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#undef min /* dammit, Visual C++! */

struct box
{
    unsigned l, w, h;
};

unsigned min(unsigned left, unsigned right)
{
    return left < right? left : right;
}

unsigned paper_area(const struct box* b)
{
    unsigned side1 = b->l * b->w,
             side2 = b->w * b->h,
             side3 = b->h * b->l,
             extra = min(min(side1, side2), side3);

    return 2 * (side1 + side2 + side3) + extra;
}

int parse_box(char* str, struct box* b) /* NOT reentrant! */
{
    char* dimstr = strtok(str, "x"), * end;

    b->l = (unsigned) strtoul(dimstr, &end, 10);
    if (dimstr == end)
        return 0;

    dimstr = strtok(NULL, "x");
    b->w = (unsigned) strtoul(dimstr, &end, 10);
    if (dimstr == end)
        return 0;

    dimstr = strtok(NULL, "");
    b->h = (unsigned) strtoul(dimstr, &end, 10);
    if (dimstr == end)
        return 0;

    return 1;
}

int main(void)
{
    char line[10];
    unsigned total_area = 0;
    struct box b;

    while(fgets(line, 10, stdin))
    {
        if (!parse_box(line, &b))
        {
            perror("Cannot parse box dimensions");
            return EXIT_FAILURE;
        }

        total_area += paper_area(&b);
    }

    if (ferror(stdin))
    {
        perror("Cannot read input");
        return EXIT_FAILURE;
    }

    printf("Total area of paper required: %u\n", total_area);

    return EXIT_SUCCESS;
}
