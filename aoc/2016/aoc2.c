#include <stdlib.h>
#include <stdio.h>

enum Direction { UP, RIGHT, DOWN, LEFT, PRESS };
typedef unsigned char Digit;

static const Digit transitions[][4] = {
  //  U,   R,   D,   L
  { 0x1, 0x1, 0x3, 0x1 }, // 1
  { 0x2, 0x3, 0x6, 0x2 }, // 2
  { 0x1, 0x4, 0x7, 0x2 }, // 3
  { 0x4, 0x4, 0x8, 0x3 }, // 4
  { 0x5, 0x6, 0x5, 0x5 }, // 5
  { 0x2, 0x7, 0xA, 0x5 }, // 6
  { 0x3, 0x8, 0xB, 0x6 }, // 7
  { 0x4, 0x9, 0xC, 0x7 }, // 8
  { 0x9, 0x9, 0x9, 0x8 }, // 9
  { 0x6, 0xB, 0xA, 0xA }, // A
  { 0x7, 0xC, 0xD, 0xA }, // B
  { 0x8, 0xC, 0xC, 0xB }, // C
  { 0xB, 0xD, 0xD, 0xD }  // D
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
  if (digit > 9)
    putchar('A' + digit - 0xA);
  else
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
