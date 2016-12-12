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

static bool check_part(const char** part, const char* end)
{
  const char* bracket = strchr(*part, '[');
  if (bracket)
    end = bracket;
  
  bool found = find_abba(*part, end);
  *part = end;
  return found;
}

static bool check_hypernet(const char** part, const char* end)
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
    
  if (check_part(&address, end))
      has = true;
    
    if (*address == '[')
    {
      ++address;
      if (!check_hypernet(&address, end))
        return false;
    }
    
    ++address;
  }
  
  return has;
}

int main(void)
{
  char address[256];
  unsigned count = 0;
  while (fgets(address, 256, stdin))
  {
    if (has_tls(address))
      ++count;
  }
  
  printf("%u addresses support TLS.\n", count);
}
