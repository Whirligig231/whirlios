#include "mlib.h"

int scompare(char str1[], char str2[]) {
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

int slength(char str[]) {
  char *s;
  for (s = str; *s; s++);
  return s - str;
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
