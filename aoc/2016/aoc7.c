#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static bool is_abba(const char* str)
{
  if (str[0] == str[1])
    return false;

  return str[0] == str[3] && str[1] == str[2];
}

static bool find_abba(const char* part, const char* end)
{
  for (; part + 3 < end; ++part)
  {
    if (is_abba(part))
      return true;
  }

  return false;
}

static bool check_supernet_abba(const char** part, const char* end)
{
  const char* bracket = strchr(*part, '[');
  if (bracket)
    end = bracket;

  bool found = find_abba(*part, end);
  *part = end;
  return found;
}

static bool check_hypernet_abba(const char** part, const char* end)
{
  const char* bracket = strchr(*part, ']');
  if (bracket)
    end = bracket;
  else
    fputs("warning: mismatched hypernet delimiter ('[')", stderr);

  bool found = find_abba(*part, end);
  *part = end;
  return !found;
}

static bool has_tls(const char* address)
{
  const char* end = address + strlen(address);

  bool has = false;
  while (address < end)
  {
    if (has && !strchr(address, '['))
      return true;

    if (check_supernet_abba(&address, end))
      has = true;

    if (*address == '[')
    {
      ++address;
      if (!check_hypernet_abba(&address, end))
        return false;
    }

    ++address;
  }

  return has;
}

static bool is_aba(const char* str)
{
  if (str[0] == str[1])
    return false;

  return str[0] == str[2];
}

static bool is_bab(const char* str, char c1, char c2)
{
  return str[0] == c2
    && str[1] == c1
    && str[2] == c2;
}

static bool find_bab(const char* address, const char* end, char c1, char c2)
{
  while ((address = strchr(address, '[')))
  {
    const char* bracket = strchr(address, ']');

    for (++address; address + 2 < bracket && address < end; ++address)
    {
      if (is_bab(address, c1, c2))
        return true;
    }

    address = bracket + 1;
  }

  return false;
}

static bool find_aba(const char* address, const char* end)
{
  const char* it = address,
    *bracket;

  do
  {
    bracket = strchr(it, '[');

    for (; (!bracket || it + 2 < bracket) && it < end; ++it)
    {
      if (is_aba(it))
      {
        if (find_bab(address, end, it[0], it[1]))
          return true;
      }
    }
  } while (bracket && (it = strchr(bracket, ']') + 1));

  return false;
}

static bool has_ssl(const char* address)
{
  const char* end = address + strlen(address);
  return find_aba(address, end);
}

int main(void)
{
  char address[256];
  unsigned tls = 0, ssl = 0;
  while (fgets(address, 256, stdin))
  {
    if (has_tls(address))
      ++tls;
    if (has_ssl(address))
      ++ssl;
  }

  printf("%u addresses support TLS.\n"
    "%u addresses support SSL.\n",
    tls, ssl);
}
