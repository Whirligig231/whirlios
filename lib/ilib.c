void iwrites(char *str) {
  syscall(0x4953, str, 0, 0);
}

void iwritec(char ch) {
  syscall(0x4943, ch, 0, 0);
}

void ireads(char *str) {
  syscall(0x4973, str, 0, 0);
}

char ireadc() {
  char ch;
  syscall(0x4963, &ch, 0, 0);
  return ch;
}

int ireadk() {
  int ch;
  syscall(0x4963, &ch, 0, 0);
  return ch;
}
