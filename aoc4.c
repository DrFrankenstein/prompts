#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincrypt.h>

#define snprintf sprintf_s /* Visual C++ and its very partial implementation of C99... */

HCRYPTPROV crypt;

size_t md5hash(char* data, size_t size, uint8_t* hash, size_t hashsiz)
{   /* A non-Windows implementation is left as an exercise... */
    HCRYPTHASH md5;

    if (!CryptCreateHash(crypt, CALG_MD5, 0, 0, &md5))
        return 0;

    if (!CryptHashData(md5, (BYTE*) data, size, 0))
        return 0;

    if (!CryptGetHashParam(md5, HP_HASHVAL, (BYTE*) hash, (DWORD*) &hashsiz, 0))
        return 0;

    CryptDestroyHash(md5);

    return hashsiz;
}

int validate_hash(const uint8_t* hash)
{
    return hash[0] == 0 && hash[1] == 0 && (hash[2] & 0xf0) == 0;
}

int try_value(const char* key, size_t keylen, unsigned value)
{
    char data[32];
    char* suffix = data + keylen;
    size_t maxsuffix = sizeof data - keylen;
    size_t datalen;
    uint8_t hash[16];
    static const size_t HASH_SIZE = 16;

    memcpy(data, key, keylen);
    datalen = snprintf(suffix, maxsuffix, "%u", value) + keylen;
    if(!md5hash(data, datalen, hash, HASH_SIZE))
        exit(EXIT_FAILURE);

    return validate_hash(hash);
}

unsigned mine(const char* key)
{
    int found;
    size_t keylen = strlen(key);
    unsigned value = 0;

    do
    {
        found = try_value(key, keylen, value);
    } while (!found && ++value != 0);

    return value;
}

int main(void)
{
    const char key[] = "iwrupvqb";
    unsigned value;
    
    if(!CryptAcquireContext(&crypt, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        return EXIT_FAILURE;

    value = mine(key);

    CryptReleaseContext(crypt, 0);

    printf("Answer is %u\n", value);

    return EXIT_SUCCESS;
}
