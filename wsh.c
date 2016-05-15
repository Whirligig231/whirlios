#include "lib/ilib.h"
#include "lib/slib.h"

int main() {
  char buffer[80];
  char verb[80];
  int len;
  iwrites("Welcome to WhirliOS.\nThis is a 16-bit OS designed by Whirligig231.\n\n");
  while (1) {
    iwrites("/> ");
    ireads(buffer);
    len = sfindc(buffer, ' ');
    if (len < 0)
      len = slength(buffer);
    scopy(verb, buffer, len);
    if (!scompare(verb, "help")) {
      iwrites("\nhelp: display this message\n\n");
    }
    else {
      iwrites("Error: unrecognized command \"");
      iwrites(verb);
      iwrites("\"\n");
    }
  }
}
