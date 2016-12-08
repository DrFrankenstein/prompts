#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "deps/md5/global.h"
#include "deps/md5/md5.h"
#include "deps/md5/md5c.c" // ugly, but I want my AoC solutions to be self-contained. remove this and build accordingly if it prevents you from sleeping at night.

static void hash_string(char* in, size_t len, unsigned char out[static 16])
{
  MD5_CTX ctx;
  MD5Init(&ctx);
  MD5Update(&ctx, (unsigned char*) in, (unsigned) len);
  MD5Final(out, &ctx);
}

static bool try_gen_char(char* in, size_t len, char* out)
{
  unsigned char hash[16];
  hash_string(in, len, hash);
  bool good = !hash[0] && !hash[1] && !(hash[2] & 0xf0);

  if (good)
  {
    printf("good hash found. key=%s, hash=", in);
    for (size_t idx = 0; idx < 16; ++idx)
      printf("%02hhx", hash[idx]);
    putchar('\n');
    
    *out = (char) (hash[2] & 0x0f);
    if (*out >= 0x0a)
      *out += 'a' - 0xa;
    else
      *out += '0';
  }
  
  return good;
}

static void crack_password(char door_id[static 32], size_t idlen, char pass[static 8])
{
  char* numstr = door_id + idlen;
  size_t numlen = 31 - idlen;
  
  size_t goodcount = 0;
  for (unsigned idx = 0; goodcount < 8 && idx < UINT_MAX; ++idx)
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
      exit(1);
    }
    
    if (try_gen_char(door_id, idlen + (size_t) written, &pass[goodcount]))
        ++goodcount;
  }
  
  if (goodcount < 8)
  {
    puts("error: cannot figure out password, ran out of indexes");
    exit(1);
  }
}

int main(void)
{
  char door_id[32];
  size_t seedlen = fread(door_id, 1, 31, stdin);
  
  if (ferror(stdin))
  {
    perror("error: cannot read input file");
    return EXIT_FAILURE;
  }
  
  char pass[8];
  crack_password(door_id, seedlen, pass);
  
  printf("The password for the security door is: %.8s\n", pass);
}
