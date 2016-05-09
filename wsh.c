#include "lib/ilib.h"

int main() {
  char buffer[80];
  iwrites("Type something: ");
  ireads(buffer);
  iwrites("You typed ");
  iwrites(buffer);
  iwrites("!\n");
  while (1) continue;
}