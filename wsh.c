#include "lib/ilib.h"
#include "lib/slib.h"
#include "lib/flib.h"

void ls();
void cd(char *dir);

int main() {
  char buffer[80];
  char verb[80];
  char wd[512];
  int len;
  iwrites("Welcome to WhirliOS.\nThis is a 16-bit OS designed by Whirligig231.\n\n");
  while (1) {
    fgetwd(wd);
    iwrites(wd);
    iwrites("> ");
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
    else if (!scompare(verb, "cd")) {
      cd(buffer + len + 1);
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
  int type;
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
    type = fgettype(f);
    if (type == 1)
      isetcolor(14);
    else if (type == 2)
      isetcolor(9);
    else if (type == 3)
      isetcolor(12);
    iwrites(name);
    iwrites("\n");
    isetcolor(7);
  }
  
  iwrites("\n");
}

void cd(char *dir) {
  char dir2[512];
  fexpand(dir2, dir);
  if (!fget(dir2)) {
    iwrites("Error: directory does not exist\n");
    return;
  }
  fsetwd(dir2);
}