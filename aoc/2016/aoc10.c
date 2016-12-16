#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef ENABLE_TRACE
#define TRACE(expr) expr
#else
#define TRACE(expr) 
#endif

struct Bot
{
  unsigned char chips[2];
  unsigned char low;
  unsigned char high;
  bool low_output : 1;
  bool high_output : 1;
  bool has_instruction : 1;
};

static struct Bot bots[UCHAR_MAX + 1];
static unsigned char outputs[UCHAR_MAX + 1];

static void assign(unsigned char chip, unsigned char id, bool is_output);

static void try_act(struct Bot* bot, unsigned char botid)
{
  if (bot->chips[0] == 0 || bot->chips[1] == 0 || !bot->has_instruction)
    return;

  unsigned char low, high;
  if (bot->chips[0] < bot->chips[1])
  {
    low = bot->chips[0];
    high = bot->chips[1];
  }
  else
  {
    low = bot->chips[1];
    high = bot->chips[0];
  }

  if (low == 17 && high == 61)
    printf("Bot %hhu compares microchips 61 and 17.\n", botid);
  
  bot->chips[0] = bot->chips[1] = 0;

  TRACE(printf("  bot %3hhu low : ", botid));
  assign(low, bot->low, bot->low_output);
  TRACE(printf("  bot %3hhu high: ", botid));
  assign(high, bot->high, bot->high_output);
}

static void assign_bot(unsigned char chip, unsigned char botid)
{
  TRACE(printf("chip %3hhu -> bot %3hhu\n", chip, botid));
  
  struct Bot* bot = &bots[botid];

  if (bot->chips[0] == 0)
    bot->chips[0] = chip;
  else
  {
    bot->chips[1] = chip;
    try_act(bot, botid);
  }
}

static void assign_output(unsigned char chip, unsigned char output)
{
  TRACE(printf("chip %3hhu -> output %3hhu\n", chip, output));
  
  if (outputs[output] != 0)
    fprintf(stderr, "warning: collision in output bin %hhu: chip %hhu replaced by chip %hhu\n", output, outputs[output], chip);

  outputs[output] = chip;
}

static void assign(unsigned char chip, unsigned char id, bool is_output)
{
  if (is_output)
    assign_output(chip, id);
  else
    assign_bot(chip, id);
}

static void set_compare(unsigned char botid, bool low_output, unsigned char low, bool high_output, unsigned char high)
{
  struct Bot* bot = &bots[botid];
  bot->low_output = low_output;
  bot->low = low;
  bot->high_output = high_output;
  bot->high = high;
  bot->has_instruction = true;
  
  try_act(bot, botid);
}

static int try_parse_assignment(void)
{
  unsigned char chip, bot;
  int status = scanf("value %hhu goes to bot %hhu ", &chip, &bot);
  if (status == 2)
  {
    TRACE(printf("\nvalue %hhu goes to bot %hhu\n", chip, bot));
    TRACE(fputs("  assigning: ", stdout));
    assign_bot(chip, bot);
  }
  else if (status != EOF && status != 0)
  {
    fputs("parse error: malformed 'value' command\n", stderr);
    exit(EXIT_FAILURE);
  }

  return status;
}

static bool parse_qualifier(const char* qualifier)
{
  if (strcmp(qualifier, "bot") == 0)
    return false;
  if (strcmp(qualifier, "output") == 0)
    return true;

  fprintf(stderr, "parse error: invalid destination qualifier '%s'\n", qualifier);
  exit(EXIT_FAILURE);
}

static int try_parse_compare(void)
{
  unsigned char bot, low, high;
  char lqualifier[7], hqualifier[7];
  int status = scanf("bot %hhu gives low to %6s %hhu and high to %6s %hhu ",
                      &bot, lqualifier, &low, hqualifier, &high);

  if (status == 5)
  {
    TRACE(printf("\nbot %hhu gives low to %s %hhu and high to %s %hhu\n", bot, lqualifier, low, hqualifier, high));
    bool low_output = parse_qualifier(lqualifier),
      high_output = parse_qualifier(hqualifier);
    set_compare(bot, low_output, low, high_output, high);
  }
  else if (status != EOF && status != 0)
  {
    fputs("parse error: malformed 'bot gives' command\n", stderr);
    exit(EXIT_FAILURE);
  }

  return status;
}

static void parse_instruction(void)
{
  int status = try_parse_assignment();
  if (status == EOF)
    return;
  if (status == 0)
    status = try_parse_compare();

  if (status == 0)
  {
    char garbage[80];
    if (!fgets(garbage, 80, stdin))
      garbage[0] = '\0';
    fprintf(stderr, "parse error: unrecognized command '%s'", garbage);
    exit(EXIT_FAILURE);
  }
}

#ifdef ENABLE_TRACE
static void dump_state(void)
{
  puts("\nBOTS\n"
       "====");
  for (unsigned short idx = 0; idx <= UCHAR_MAX; ++idx)
  {
    struct Bot* bot = &bots[idx];
    if (bot->chips[0] == 0)
      continue;
    printf("%3hu: %3hhu", idx, bot->chips[0]);
    
    if (bot->chips[1] != 0)
    {
      printf(", %3hhu (%s)", bot->chips[1],
             bot->has_instruction? "MISSED INSTRUCTION" : "no instruction");
    }
    
    putchar('\n');
  }
  
  puts("\nOUTPUTS\n"
       "=======");
  for (unsigned short idx = 0; idx <= UCHAR_MAX; ++idx)
  {
    unsigned char chip = outputs[idx];
    if (chip)
      printf("%3hu: %3hhu\n", idx, chip);
  }
}
#endif

int main(void)
{
  TRACE(atexit(dump_state));
  
  while (!feof(stdin))
    parse_instruction();
  
  int prod = outputs[0] * outputs[1] * outputs[2];
  printf("The product of outputs 0, 1 and 2 is %d.\n", prod);

  return EXIT_SUCCESS;
}
