#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

/* double parentheses don't disable the assignment-in-condition warning in CL */
#pragma warning(disable:4706)

struct eval_ctx
{
    FILE* stream;
    const char* err;
    char allowed_nums[4];
};

static int mark_number(char allowed[4], char number)
{
    char* loc = memchr(allowed, number, sizeof allowed);
    if (loc)
    {
        *loc = -1;
        return 1;
    }
    else return 0;
}

void consume_line(FILE* stream)
{
    int c;
    while ((c = getc(stream)) != '\n' && c != EOF) ;
}

static char eval_consume(struct eval_ctx* ctx, const char* expected)
{
    int c;
    while ((c = getc(ctx->stream)) != '\n' && isspace(c)) ;    /* skip whitespace except LF */

    if (c == EOF || c == '\n' || !strchr(expected, c))
    {   /* we don't need this character, put it back */
        ungetc(c, ctx->stream);
        return '\0';
    }

    return (char) c;
}

static double eval_expr(struct eval_ctx* ctx);

static double eval_value(struct eval_ctx* ctx)
{
    char tok;
    double res;

    if (eval_consume(ctx, "("))
    {   /* got a parenthesis, evaluate its contents and use it as value */
        res = eval_expr(ctx);
        if (!eval_consume(ctx, ")"))
        {
            ctx->err = "Parenthesis not closed";
            return 0.;
        }
        return res;
    }

    if ((tok = eval_consume(ctx, "123456789")))
    {   /* got a number (we only support single-digit numbers) */
        char num = tok - '0';   /* convert from character to number */
        if (mark_number(ctx->allowed_nums, num))
        {
            res = (double) num;
            return res;
        }
        else
        {
            ctx->err = "Number not allowed";
            /*ctx->cur--; /* report error at right spot */
            return 0.;
        }
    }

    /* got something we can't parse as a value */
    ctx->err = "Unexpected character";
    return 0.;
}

static double eval_multiplicative(struct eval_ctx* ctx)
{
    double left = eval_value(ctx),
        right;
    char tok;
    if (ctx->err) return 0.;
    while ((tok = eval_consume(ctx, "*/")))
    {
        right = eval_value(ctx);
        if (ctx->err) return 0.;
        switch (tok)
        {
        case '*':
            left *= right;
            break;
        case '/':
            left /= right;
            break;
        }
    }

    /* didn't find a * or /, just return the first value we read */
    return left;
}

static double eval_additive(struct eval_ctx* ctx)
{
    /* * and / have precedence, evaluate them before coming back here */
    double left = eval_multiplicative(ctx),
        right;
    char tok;
    if (ctx->err) return 0.;
    while ((tok = eval_consume(ctx, "+-")))
    {
        right = eval_multiplicative(ctx);
        if (ctx->err) return 0.;
        switch (tok)
        {
        case '+':
            left += right;
            break;
        case '-':
            left -= right;
            break;
        }
    }
    
    /* didn't find a + or -, just return the first value we read */
    return left;
}

static double eval_expr(struct eval_ctx* ctx)
{
    return eval_additive(ctx);
}

double eval(FILE* stream, char allowed_nums[4], char* ok)
{
    int i, c;
    double result;
    struct eval_ctx ctx;
    ctx.stream = stream;
    ctx.err = NULL;

    /* we make a copy instead of pointing to the original array so that the can
       remove the numbers when we see them in the expression */
    memcpy(ctx.allowed_nums, allowed_nums, sizeof allowed_nums);

    result = eval_expr(&ctx);

    if((c = getc(stream)) != '\n' && c != EOF)
    {   /* still got characters in the stream */
        if(!ctx.err)
            ctx.err = "Unexpected character";

        consume_line(stream);
    }

    for (i = 0; !ctx.err && i < 4; i++)
        if(ctx.allowed_nums[i] != -1)
            ctx.err = "Not all numbers were used";

    if (ctx.err)
    {
        puts(ctx.err);
        *ok = 0;
        return 0.;
    }

    *ok = 1;
    return result;
}

int validate_answer(double answer)
{
    return fabs(24 - answer) < 0.000001; /* eh, close enough */
}

int turn(void)
{
    char numbers[4], ok;
    int good = 0, i;
    double result;
    struct eval_ctx ctx;
    ctx.stream = stdin;

    for(i = 0; i < 4; i++)
        numbers[i] = rand() % 9 + 1;

    do
    {
        printf("\nYour numbers are %hhd, %hhd, %hhd and %hhd.\nMake 24 [expr/s/q]: ",
            numbers[0], numbers[1], numbers[2], numbers[3]);

        if (eval_consume(&ctx, "Ss"))
        {
            consume_line(stdin);
            return 1;
        }
        if (eval_consume(&ctx, "Qq"))
        {
            consume_line(stdin);
            return 0;
        }

        result = eval(stdin, numbers, &ok);

        if(!ok) continue;

        printf("Result is %f. ", result);

        if((good = validate_answer(result)))
            puts("Correct!");
        else
            puts("Wrong!");

    } while (!good);

    return 1;
}

int main(void)
{
    puts("THE 24 GAME");
    puts("I will draw four numbers. Using these four numbers, write an \n"
        "expression that gives 24.");
    puts("You can use + (addition), - (subtraction), * (multiplication), \n"
        "/ (division) and group operations in parentheses. PMDAS applies.");
    puts("You can skip a turn by typing \"S\" and quit with \"Q\".");

    srand((unsigned) time(NULL));

    while (turn()) ;

    return EXIT_SUCCESS;
}
