#include "zerynth.h"

char *strstr(const char *s1, const char *s2){
    size_t n = strlen(s2);
    while(*s1)
        if(!memcmp(s1++,s2,n))
            return s1-1;
    return 0;
}

int strncmp(const char *s1, const char *s2, register size_t n)
{
  register unsigned char u1, u2;

  while (n-- > 0)
    {
      u1 = (unsigned char) *s1++;
      u2 = (unsigned char) *s2++;
      if (u1 != u2)
    return u1 - u2;
      if (u1 == '\0')
    return 0;
    }
  return 0;
}

int strcmp(const char *s1, const char *s2)
{
  for ( ; *s1 == *s2; s1++, s2++)
    if (*s1 == '\0')
      return 0;
  return ((*(unsigned char *)s1 < * (unsigned char *)s2) ? -1 : +1);
}

char *strchr(const char *s, int c)
{
    while (*s != (char)c)
        if (!*s++)
            return 0;
    return (char *)s;
}

#define isdigit(x) ((x)>='0' && (x)<='9')
int atoi(const char *c)
{
  int value = 0;
  int sign = 1;
  if ( *c == '+' || *c == '-' )
  {
    if ( *c == '-' ) sign = -1;
    c++;
  }
  while (isdigit(*c))
  {
    value *= 10;
    value += (int) (*c - '0');
    c++;
  }
  return (value * sign);
}

char *strcat(char*s1, const char*s2) {
  memcpy(s1 + strlen(s1), s2, strlen(s2) + 1);
  return s1;
}

char* strcpy(char *dst0, const char *src0) {
  char *s = dst0;
  while (*dst0++ = *src0++) {
  }
  return s;
}

char* strncpy(char *dest, const char *src, size_t n) {
  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for ( ; i < n; i++)
    dest[i] = '\0';

  return dest;
}

char *strncat(char *dest, const char *src, size_t n)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while (n--)
        if (!(*dest++ = *src++))
            return ret;
    *dest = 0;
    return ret;
}