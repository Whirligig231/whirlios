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