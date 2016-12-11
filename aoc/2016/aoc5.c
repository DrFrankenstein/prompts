#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#define PROTOTYPES 1
#include "deps/md5/global.h"
#include "deps/md5/md5.h"
#include "deps/md5/md5c.c" // ugly, but I want my AoC solutions to be self-contained. remove this and build accordingly if it prevents you from sleeping at night.

// CL doesn't support static array arguments
#ifdef _MSC_VER
# define STATIC_ARRAY_SIZE
#else
# define STATIC_ARRAY_SIZE static
#endif

static void output_status(unsigned char hash[STATIC_ARRAY_SIZE 16], char pass[STATIC_ARRAY_SIZE 8])
{
  putchar('\r');
  for (size_t i = 0; i < 16; ++i)
    printf("%02hhx", hash[i]);

  printf(" ... H4XX1NG P455W0RD ... %.8s", pass);
}

static void hash_string(char* in, size_t len, unsigned char out[STATIC_ARRAY_SIZE 16])
{
  MD5_CTX ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, (unsigned char*) in, (unsigned) len);
  MD5Final(out, &ctx);
}

static void try_gen_char(char* in, size_t len, char pass[STATIC_ARRAY_SIZE 8], unsigned idx)
{
  unsigned char hash[16];
  hash_string(in, len, hash);
  
  unsigned char position = (unsigned char) (hash[2] & 0x0f);
  if (position < 8 && pass[position] == '?' &&
      !hash[0] && !hash[1] && !(hash[2] & 0xf0))
  {
    char ch = (char) (hash[3] >> 4);
    if (ch >= 0x0a)
      pass[position] = ch + 'a' - 0xa;
    else
      pass[position] = ch + '0';

    output_status(hash, pass);
  }
  else if (!(idx & 0x000003ff))
    output_status(hash, pass);
}

static void crack_password(char door_id[STATIC_ARRAY_SIZE 32], size_t idlen, char pass[STATIC_ARRAY_SIZE 8])
{
  char* numstr = door_id + idlen;
  size_t numlen = 31 - idlen;
  
  memset(pass, '?', 8);
  for (unsigned idx = 0; idx < UINT_MAX && memchr(pass, '?', 8); ++idx)
  {
    int written = snprintf(numstr, numlen + 1, "%u", idx);
    
    if (written < 0)
    {
      perror("error: cannot generate hash key");
      abort();
    }
    
    if ((size_t) written > numlen)
    {
      puts("error: cannot figure out password, ran out of space for the index");
      exit(EXIT_FAILURE);
    }
    
    try_gen_char(door_id, idlen + (size_t) written, pass, idx);
  }
  
  if (memchr(pass, '?', 8))
  {
    puts("error: cannot figure out password, ran out of indexes");
    exit(EXIT_FAILURE);
  }
}

int main(void)
{
  char door_id[32];
  size_t idlen = fread(door_id, 1, 31, stdin);
  
  if (ferror(stdin))
  {
    perror("error: cannot read input file");
    return EXIT_FAILURE;
  }
  
  char pass[8];
  crack_password(door_id, idlen, pass);
  
  putchar('\n');
}
