void iwrites(char *str) {
  interrupt(0x21, 0x4953, str, 0, 0);
}

void iwritec(char ch) {
  interrupt(0x21, 0x4943, ch, 0, 0);
}

void ireads(char *str) {
  interrupt(0x21, 0x4973, str, 0, 0);
}

char ireadc() {
  char ch;
  interrupt(0x21, 0x4963, &ch, 0, 0);
  return ch;
}

int ireadk() {
  int ch;
  interrupt(0x21, 0x4963, &ch, 0, 0);
  return ch;
}