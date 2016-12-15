#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum ScreenSize { SCR_WIDTH = 50, SCR_HEIGHT = 6 };
typedef uint64_t Screen[SCR_HEIGHT];
static const uint64_t ROWMASK = ((uint64_t) 1 << SCR_WIDTH) - 1;

enum OpCode { INVALID, RECT, SHIFTROW, SHIFTCOL };
struct Instruction { enum OpCode opcode; unsigned p1, p2; };

static void render(Screen screen)
{
  for (size_t row = 0; row < 6; ++row)
  {
    uint64_t pixels = screen[row];
    for (unsigned col = 0; col < 50; ++col, pixels >>= 1)
      putchar(pixels & 1? '#' : '.');
    putchar('\n');
  }
}

static void fill_rect(Screen screen, unsigned char w, unsigned char h)
{
  uint64_t pattern = (((uint64_t) 1 << w) - 1);
  
  while (h--)
    screen[h] |= pattern & ROWMASK;
}

static void shift_row(Screen screen, unsigned char row, unsigned by)
{
  uint64_t rpart = screen[row] << by,
           lpart = screen[row] >> (50 - by);
  
  screen[row] = (lpart | rpart) & ROWMASK;
}

static void write_pixel(uint64_t* dst, uint64_t mask, uint8_t pixel)
{
  if (pixel)
    *dst |= mask;
  else
    *dst &= ~mask;
}

static void shift_col(Screen screen, unsigned char col, unsigned by)
{
  uint64_t mask = (uint64_t) 1 << col;
  
  uint8_t pixels = 0;
  for (size_t row = 0; row < SCR_HEIGHT; ++row)
    if (screen[row] & mask)
      pixels |= (uint8_t) (1 << row);
  
  for (size_t row = 0; row < SCR_HEIGHT; ++row)
    write_pixel(screen + (row + by) % SCR_HEIGHT, mask, (uint8_t) (pixels & 1 << row));
}

static struct Instruction parse_rect(void)
{
  struct Instruction instruction = { RECT, 0, 0 };
  int status = scanf("%ux%u", &instruction.p1, &instruction.p2);
  if (status < 2)
  {
    fputs("error: parse error in 'rect' command\n", stderr);
    instruction.opcode = INVALID;
  }
  
  return instruction;
}

static struct Instruction parse_shift(void)
{
  struct Instruction instruction;
  char type[7];
  char coord;
  int status = scanf("%6s %[xy]=%u by %u", type, &coord, &instruction.p1, &instruction.p2);
  if (status < 3)
  {
    fputs("error: parse error in 'rotate' command\n", stderr);
    return (struct Instruction) { INVALID, 0, 0 };
  }
  
  if (strcmp(type, "row") == 0)
  {
    if (coord != 'y')
      fprintf(stderr, "warning: invalid coordinate identifier '%c' in 'rotate row' command. assuming 'y'\n", coord);
    instruction.opcode = SHIFTROW;
  }
  else if (strcmp(type, "column") == 0)
  {
    if (coord != 'x')
      fprintf(stderr, "warning: invalid coordinate identifier '%c' in 'rotate column' command. assuming 'x'\n", coord);
    instruction.opcode = SHIFTCOL;
  }
  else
  {
    fprintf(stderr, "error: invalid shift type '%s'\n", type);
    instruction.opcode = INVALID;
  }
  
  return instruction;
}

static struct Instruction parse_instruction(void)
{
  char command[7];
  int status = scanf("%6s", command);
  if (status < 1)
    return (struct Instruction) { INVALID, 0, 0 };
  
  if (strcmp(command, "rect") == 0)
    return parse_rect();
  if (strcmp(command, "rotate") == 0)
    return parse_shift();
  
  fprintf(stderr, "invalid command '%s'\n", command);
  return (struct Instruction) { INVALID, 0, 0 };
}

static void paint_image(Screen screen)
{
  while (!feof(stdin))
  {
    struct Instruction instruction = parse_instruction();
    
    switch (instruction.opcode)
    {
      case RECT:
        fill_rect(screen, (unsigned char) instruction.p1, (unsigned char) instruction.p2);
        break;
      case SHIFTROW:
        shift_row(screen, (unsigned char) instruction.p1, instruction.p2);
        break;
      case SHIFTCOL:
        shift_col(screen, (unsigned char) instruction.p1, instruction.p2);
        break;
      case INVALID:
        if (!feof(stdin))
        fputs("warning: skipped invalid instruction\n", stderr);
    }
  }
}

static unsigned popcount(uint64_t n)
{
  n = ((n & 0xAAAAAAAAAAAAAAAA) >>  1) + (n & 0x5555555555555555);
  n = ((n & 0xCCCCCCCCCCCCCCCC) >>  2) + (n & 0x3333333333333333);
  n = ((n & 0xF0F0F0F0F0F0F0F0) >>  4) + (n & 0x0F0F0F0F0F0F0F0F);
  n = ((n & 0xFF00FF00FF00FF00) >>  8) + (n & 0x00FF00FF00FF00FF);
  n = ((n & 0xFFFF0000FFFF0000) >> 16) + (n & 0x0000FFFF0000FFFF);
  n = ( n                       >> 32) + (n & 0x00000000FFFFFFFF);
  
  return (unsigned) n;
}

static unsigned pixcount(Screen screen)
{
  unsigned count = 0;
  for (size_t row = 0; row < SCR_HEIGHT; ++row)
    count += popcount(screen[row]);
  return count;
}

int main(void)
{  
  Screen screen = { 0 };
  paint_image(screen);
  
  render(screen);
  printf("%u pixels are lit.\n", pixcount(screen));
}
