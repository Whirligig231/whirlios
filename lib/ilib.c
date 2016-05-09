void iwrites(char *str) {
  interrupt(0x21, 0x4953, str, 0, 0);
}