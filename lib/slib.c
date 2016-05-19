#include "mlib.h"

int scompare(char *str1, char *str2) {
  int pos = 0;

  while (str1[pos] || str2[pos]) {
    if (str1[pos] < str2[pos]) {
      return -1;
    }
    else if (str1[pos] > str2[pos]) {
      return 1;
    }
    pos++;
  }

  return 0;
}

int slength(char *str) {
  char *s;
  for (s = str; *s; s++);
  return s - str;
}

void scopy(char *dest, char *src, int len) {
  int i;
  for (i = 0; i < len && src[i]; i++) {
    dest[i] = src[i];
  }
  dest[i] = '\0';
}

void sconcat(char *dest, char *src) {
  scopy(dest + slength(dest), src, slength(src));
}

void strunc(char *str, int len) {
  if (slength(str) > len)
    str[len] = '\0';
}

int sfindc(char *str, char ch) {
  int i;
  for (i = 0; str[i]; i++)
    if (str[i] == ch)
      return i;
    
  return -1;
}

int sfinds(char *str1, char *str2) {
  /* TODO: Implement KMP for better efficiency once there's some form of dynamic allocation */
  int i, j;
  int matched;
  for (i = 0; str1[i]; i++) {
    matched = 1;
    for (j = 0; str1[i + j] || str2[j]; j++)
      if (str1[i + j] != str2[i + j])
        matched = 0;
    if (matched)
      return i;
  }
  return -1;
}

int sfindlc(char *str, char ch) {
  int i;
  int ret = -1;
  for (i = 0; str[i]; i++)
    if (str[i] == ch)
      ret = i;
    
  return ret;
}

void sformati(char *buffer, int n) {
  int pos = 0;
  int n_temp;

  if (n == 0) {
    buffer[0] = '0';
    buffer[1] = '\0';
    return;
  }

  if (n < 0){
    n = -n;
    buffer[0] = '-';
    pos++;
  }

  n_temp = n;
  while (n_temp > 0) {
    n_temp = mdiv(n_temp, 10);
    pos++;
  }

  buffer[pos] = '\0';
  while (n > 0) {
    pos--;
    buffer[pos] = '0' + mmod(n, 10);
    n = mdiv(n, 10);
  }
}

void sformath(char *buffer, char *hexes, int len) {
  int i;
  char *digits = "0123456789ABCDEF";
  for (i = 0; i < len; i++) {
    if (i > 0)
      buffer[3*i-1] = ' ';
    buffer[3*i] = digits[hexes[i] >> 4];
    buffer[3*i+1] = digits[hexes[i] & 15];
  }
  buffer[3*i-1] = '\0';
}