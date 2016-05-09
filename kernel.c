void runkernel();

int main() {
	runkernel();
}

int div(int a, int b) {
  int quot;
  int q;
  int c;
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
  return quot;
}

int mod(int a, int b) {
  int c;
  while (a >= b) {
    c = b;
    do {
      c <<= 1;
    } while (c <= a && c < 8192);
    c >>= 1;
    a -= c;
  }
  return a;
}

void printChar(char c) {
  interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void printString(char *str) {
  int i;
  for (i = 0; str[i]; i++)
    printChar(str[i]);
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x0201, buffer, ((div(sector, 36) << 8) | mod(sector, 18)) + 1, (div(sector, 18) & 1) << 8);
}

int fileGetSector(char *buffer, int file, int *sector) {
  /* TODO: Make this not suck as much (currently it does two reads sometimes, which could theoretically be helped) */
  int indexFlags;
  char buf[512];
  int i;
  int index;
  readSector(buf, file);
  indexFlags = ((buf[0] & 0x80) >> 6) + ((buf[1] & 0x80) >> 7);
  if (indexFlags == 0) {
    /* Unindexed file */
    if (*sector)
      return 1; /* EOF */
    for (i = 0; i < 504; i++) {
      buffer[i] = buf[i+8];
    }
    for (; i < 512; i++)
      buffer[i] = 0;
  } else if (indexFlags == 1) {
    /* Singly indexed file */
    if (*sector < 0 || *sector >= 336)
      return 1; /* EOF */
    if ((*sector) & 1) {
      index = ((buf[9+3*(*sector >> 1)] & 0x0F) << 8) + buf[10+3*(*sector >> 1)];
    } else {
      index = (buf[8+3*(*sector >> 1)] << 4) + ((buf[9+3*(*sector >> 1)] & 0xF0) >> 4);
    }
    if (!index)
      return 1; /* EOF */
    readSector(buffer, index);
  } else {
    return 2; /* Unsupported indexing mode */
  }
  *sector++;
  return 0;
}

int handleInterrupt21() {
  /* Placeholder */
}

int handleTimerInterrupt() {
  /* Placeholder */
}

void runkernel() {
  char buf[512];
  int sec;
  int n;
  sec = 0;
  n = fileGetSector(buf, 17, &sec);
  printString(buf);
  while (1) continue;
}