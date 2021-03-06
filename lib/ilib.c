#include "flib.h"

void iwrites(char *str) {
  syscall(0x4953, str, 0, 0);
}

void iwritec(char ch) {
  syscall(0x4943, ch, 0, 0);
}

void isetcolor(char col) {
  syscall(0x494B, col, 0, 0);
}

void isetpos(int row, int col) {
  syscall(0x4950, row, col, 0);
}

void ireads(char *str, int maxlen) {
  syscall(0x4973, str, 0, maxlen);
}

void ireadps(char *str, int maxlen) {
  syscall(0x4973, str, 1, maxlen);
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

int iexec(file prog) {
  int ch;
  syscall(0x4958, prog, &ch, 0);
  return ch;
}

void iexit() {
  syscall(0x4978, 0, 0, 0);
}

void ireboot() {
  syscall(0x4952, 0, 0, 0);
}