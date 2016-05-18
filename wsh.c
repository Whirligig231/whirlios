#include "lib/ilib.h"
#include "lib/slib.h"
#include "lib/flib.h"

void ls();

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
      iwrites("\nhelp: display this message\nls: list files\n\n");
    }
    else if (!scompare(verb, "ls")) {
      ls();
    }
    else {
      iwrites("Error: unrecognized command \"");
      iwrites(verb);
      iwrites("\"\n");
    }
  }
}

void ls() {
  char path[512];
  char name[9];
  file dir;
  file f;
  int i;
  filecache c;
  
  finitcache(&c);
  
  fgetwd(path);
  iwrites("\n-- Directory ");
  iwrites(path);
  iwrites(" --\n\n");
  dir = fget(path);
  
  for (i = 0; 1; i++) {
    f = fdirget(dir, i, &c);
    if (!f)
      break;
    fgetname(name, f);
    iwrites(name);
    iwrites("\n");
  }
  
  iwrites("\n");
}