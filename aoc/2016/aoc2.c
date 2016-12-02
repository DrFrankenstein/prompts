#include <stdlib.h>
#include <stdio.h>

enum Direction { UP, RIGHT, DOWN, LEFT, PRESS };
typedef unsigned char Digit;

static const Digit transitions[][4] = {
  //U, R, D, L
  { 1, 2, 4, 1 }, // 1
  { 2, 3, 5, 1 }, // 2
  { 3, 3, 6, 2 }, // 3
  { 1, 5, 7, 4 }, // 4
  { 2, 6, 8, 4 }, // 5
  { 3, 6, 9, 5 }, // 6
  { 4, 8, 7, 7 }, // 7
  { 5, 9, 8, 7 }, // 8
  { 6, 9, 9, 8 }  // 9
};

static
Digit
move(Digit digit, enum Direction direction)
{
  return transitions[digit - 1][direction];
}

static
void
print_digit(Digit digit)
{
  putchar('0' + digit);
}

static
enum Direction
parse_instruction(int instruction)
{
  //printf("instruction: %c, ", instruction);
  switch (instruction)
  {
    case 'U': return UP;
    case 'R': return RIGHT;
    case 'D': return DOWN;
    case 'L': return LEFT;
    case '\n': return PRESS;
    default:
      fprintf(stderr, "invalid direction '%c'\n", instruction);
      abort();
  }
}

static
void
read_instructions(void)
{
  Digit digit = 5;
  int instruction;
  while ((instruction = getchar()) != EOF)
  {
    enum Direction direction = parse_instruction(instruction);
    if (direction == PRESS)
      print_digit(digit);
    else
      digit = move(digit, direction);

    //printf("digit: %hhu\n", digit);
  }
}

int
main(void)
{
  fputs("The bathroom code is: ", stdout);
  read_instructions();
  putchar('\n');
  
  return EXIT_SUCCESS;
}

