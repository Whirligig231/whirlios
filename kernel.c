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

int handleInterrupt21() {
  /* Placeholder */
}

int handleTimerInterrupt() {
  /* Placeholder */
}

void runkernel() {
  char buf[512];
  readSector(buf, 16);
  printString(buf);
  while (1) continue;
}