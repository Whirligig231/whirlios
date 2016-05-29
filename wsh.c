#include "lib/ilib.h"
#include "lib/slib.h"
#include "lib/flib.h"

void ls();
void cd(char *dir);
void cat(char *fname);
void echo(char *str);
void pwd();

int main() {
  char buffer[80];
  char verb[80];
  char wd[512];
  int len;
  int prog;
  while (1) {
    fgetwd(wd);
    iwrites(wd);
    iwrites("> ");
    ireads(buffer, 80);
    len = sfindc(buffer, ' ');
    if (len < 0)
      len = slength(buffer);
    scopy(verb, buffer, len);
    if (!scompare(verb, "help")) {
      iwrites("\ncat: print file contents\ncd: change directories\necho: print a string\nhelp: display this message\nls: list files\npwd: print current working directory\n\n");
    }
    else if (!scompare(verb, "ls")) {
      ls();
    }
    else if (!scompare(verb, "cd")) {
      cd(buffer + len + 1);
    }
    else if (!scompare(verb, "cat")) {
      /* Mrew? */
      cat(buffer + len + 1);
    }
    else if  (!scompare(verb, "echo")){
      echo(buffer + len + 1);
    }
    else if  (!scompare(verb, "pwd")){
      pwd(buffer + len + 1);
    }
    else if (!scompare(verb, "reboot")) {
      ireboot();
    }
    else if (sfindc(verb, '/') == -1) {
      prog = fget("/bin/");
      if (!prog) {
        iwrites("Error: unable to locate bin directory\n");
        continue;
      }
      prog = ffind(prog, verb);
      if (!prog) {
        iwrites("Error: unrecognized command \"");
        iwrites(verb);
        iwrites("\"\n");
        continue;
      }
      if (fgettype(prog) == 2)
        iexec(prog);
      else {
        iwrites("Error: file is not executable\n");
        continue;
      }
    }
    else if (prog = fget(verb)) {
      if (fgettype(prog) == 2)
        iexec(prog);
      else {
        iwrites("Error: file is not executable\n");
        continue;
      }
    }
    else {
      iwrites("Error: executable not found\n");
      continue;
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
  int f;
  fexpand(dir2, dir);
  f = fget(dir2);
  if (!f) {
    iwrites("Error: directory does not exist\n");
    return;
  }
  if (fgettype(f) != 3) {
    iwrites("Error: file is not a directory\n");
    return;
  }
  fsetwd(dir2);
}

void cat(char *fname) {
  file f;
  int type;
  char buf[512];
  char buffer[1536];
  int i;
  int errcode;
  f = fget(fname);
  if (!f) {
    iwrites("Error: file does not exist\n");
    return;
  }
  type = fgettype(f);
  if (type == 3) {
    iwrites("Error: file is a directory\n");
    return;
  }
  else if (type == 0) {
    for (i = 0; 1; i++) {
      errcode = freads(buffer, f, i);
      if (errcode == 2) {
        iwrites("Error: unable to read file\n");
        return;
      }
      else if (errcode == 1)
        break;
      buffer[512] = '\0';
      iwrites(buffer);
    }
  }
  else {
    for (i = 0; 1; i++) {
      errcode = freads(buf, f, i);
      if (errcode == 2) {
        iwrites("Error: unable to read file\n");
        return;
      }
      else if (errcode == 1)
        break;
      sformath(buffer, buf, 512);
      if (i > 0)
        iwritec(' ');
      iwrites(buffer);
    }
  }
  iwrites("\n");
}

void echo(char *str) {
  int len = slength(str);

  if (len <= 0) {
    iwrites("Error: No string was provided");
  }
  else {
    iwrites(str);
  }

  iwrites("\n");
}

void pwd() {
  char wd[512];
  fgetwd(wd);

  iwrites(wd);
  iwrites("\n");
}
