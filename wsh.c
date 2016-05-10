#include "lib/ilib.h"

int main() {
  char buffer[80];
  iwrites("Welcome to WhirliOS.\nThis is a 16-bit OS designed by Whirligig231.\n");
  iwrites("Type something: ");
  ireads(buffer);
  iwrites("You typed ");
  iwrites(buffer);
  iwrites("!\n");
  while (1) continue;
}
