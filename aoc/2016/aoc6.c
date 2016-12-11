#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

enum Constants
{
  CHARSET_SIZE = 1 << CHAR_BIT,
  MESSAGE_LEN = 10
};

// you could save almost 90% of the memory usage (at lest) by only
// supporting lowercase letters. this is left as an exercise to the
// reader
typedef unsigned short ErrorCorrectionState[CHARSET_SIZE][MESSAGE_LEN];
ErrorCorrectionState ec_state;

static void add_repetition(ErrorCorrectionState state, const char* message, size_t len)
{
  for (size_t idx = 0; idx < len; ++idx)
  {
    char c = message[idx];
    ++state[(unsigned char) c][idx];
  }
}

static void decode_message(ErrorCorrectionState state, char* message, size_t len)
{
  for (size_t idx = 0; idx < len; ++idx)
  {
    unsigned short min_freq = USHRT_MAX;
    char decoded = '\0';
    for (short c = 0; c < CHARSET_SIZE; ++c)
    {
      unsigned short freq = state[c][idx];
      if (freq != 0 && freq < min_freq)
      {
        min_freq = freq;
        decoded = (char) c;
      }
    }

    message[idx] = decoded;
  }
}

int main(void)
{
  char message[MESSAGE_LEN + 2]; // room for newline and null
  size_t maxlen = 0;
  while (fgets(message, MESSAGE_LEN + 2, stdin))
  {
    size_t len = strlen(message);
    if (len > maxlen)
      maxlen = len;

    add_repetition(ec_state, message, len);
  }

  decode_message(ec_state, message, maxlen);

  printf("The message is: %s\n", message);
}
