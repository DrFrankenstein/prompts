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

unsigned ribbon_len(const struct box* b)
{
    unsigned factor;
    if (b->l < b->w)
    {
        factor = b->l;
        if (b->w < b->h)
            factor += b->w;
        else 
            factor += b->h;
    }
    else
    {
        factor = b->w;
        if (b->l < b->h)
            factor += b->l;
        else
            factor += b->h;
    }

    return 2 * factor + b->l * b->w * b->h;
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
    unsigned paper = 0, ribbon = 0;
    struct box b;

    while(fgets(line, 10, stdin))
    {
        if (!parse_box(line, &b))
        {
            perror("Cannot parse box dimensions");
            return EXIT_FAILURE;
        }

        paper += paper_area(&b);
        ribbon += ribbon_len(&b);
    }

    if (ferror(stdin))
    {
        perror("Cannot read input");
        return EXIT_FAILURE;
    }

    printf("Total area of paper required: %u ft\n", paper);
    printf("Total length of ribbon required: %u ft\n", ribbon);

    return EXIT_SUCCESS;
}
