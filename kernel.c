void runkernel();

int main() {
	runkernel();
}

int handleInterrupt21() {
  /* Placeholder */
}

int handleTimerInterrupt() {
  /* Placeholder */
}

void printChar(char c) {
  interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void printString(char *str) {
  int i;
  for (i = 0; str[i]; i++)
    printChar(str[i]);
}

void runkernel() {
  printString("Hello World!");
  while (1) continue;
}