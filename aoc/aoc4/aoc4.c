#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "md5.h" /* see RFC1321 appendix A for required files */

#define snprintf sprintf_s /* Visual C++ and its very partial implementation of C99... */

typedef int (*hash_validator_t)(const uint8_t*);

size_t md5hash(char* data, size_t size, uint8_t* hash)
{
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, data, size);
    MD5Final(hash, &ctx);
}

int validate_hash5(const uint8_t* hash)
{
    return hash[0] == 0 && hash[1] == 0 && (hash[2] & 0xf0) == 0;
}

int validate_hash6(const uint8_t* hash)
{
    return hash[0] == 0 && hash[1] == 0 && hash[2] == 0;
}

int try_value(const char* key, size_t keylen, unsigned value, hash_validator_t validate)
{
    char data[32];
    char* suffix = data + keylen;
    size_t maxsuffix = sizeof data - keylen;
    size_t datalen;
    uint8_t hash[16];

    memcpy(data, key, keylen);
    datalen = snprintf(suffix, maxsuffix, "%u", value) + keylen;
    md5hash(data, datalen, hash);

    return validate(hash);
}

unsigned mine(const char* key, hash_validator_t validate)
{
    int found;
    size_t keylen = strlen(key);
    unsigned value = 0;

    do
    {
        found = try_value(key, keylen, value, validate);
    } while (!found && ++value != 0);

    return value;
}

int main(void)
{
    const char key[] = "iwrupvqb";
    unsigned value;

    value = mine(key, validate_hash5);
    printf("Answer for part 1 is %u\n", value);
    value = mine(key, validate_hash6);
    printf("Answer for part 2 is %u\n", value);

    return EXIT_SUCCESS;
}
