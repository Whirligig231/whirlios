#include "lib/ilib.h"

int main() {
  char buffer[80];
  iwrites("Welcome to WhirliOS.\nThis is a 16-bit OS designed by Whirligig231.\n\n");
  while (1) {
    iwrites("/> ");
    ireads(buffer);
    iwrites("Error: unrecognized command \"");
    iwrites(buffer);
    iwrites("\"\n");
  }
}
