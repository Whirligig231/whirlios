#include "mlib.h"

/* This is needed by BCC for structs ... for some inane reason */
/* TODO: put this somewhere else so that I don't have to copy it to every file with structs */
void memcpy(char *dest, char *src, int n) {
  int i;
  for (i = 0; i < n; i++)
    dest[i] = src[i];
}

int mdiv(int a, int b) {
  int quot;
  int q;
  int c;
  if (b < 0)
    return mdiv(-a, -b);
  if (b == 0)
    return 0;
  quot = 0;
  while (a >= b) {
    c = b;
    q = 1;
    do {
      c <<= 1;
      q <<= 1;
    } while (c <= a && c < 8192);
    c >>= 1;
    q >>= 1;
    a -= c;
    quot += q;
  }
  while (a < 0) {
    c = b;
    q = 1;
    do {
      c <<= 1;
      q <<= 1;
    } while (c <= b-a && c < 8192);
    c >>= 1;
    q >>= 1;
    a += c;
    quot -= q;
  }
  return quot;
}

int mmod(int a, int b) {
  int c;
  if (b < 0)
    return mmod(-a, -b);
  if (b == 0)
    return a;
  while (a >= b) {
    c = b;
    do {
      c <<= 1;
    } while (c <= a && c < 8192);
    c >>= 1;
    a -= c;
  }
  while (a < 0) {
    c = b;
    do {
      c <<= 1;
    } while (c <= b-a && c < 8192);
    c >>= 1;
    a += c;
  }
  return a;
}

lint maddl(lint a, lint b) {
  unsigned int a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4;
  lint c;
  a1 = a.high >> 8;
  a2 = a.high & 255;
  a3 = a.low >> 8;
  a4 = a.low & 255;
  b1 = a.high >> 8;
  b2 = a.high & 255;
  b3 = a.low >> 8;
  b4 = a.low & 255;
  
  c1 = a1 + b1;
  c2 = a2 + b2;
  c3 = a3 + b3;
  c4 = a4 + b4;
  
  c.low = (c3 << 8) + c4;
  c.high = (c3 >> 8) + c2 + (c1 << 8);
  
  return c;
}
