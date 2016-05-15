#include "flib.h"
#include "slib.h"

file froot() {
  int ret;
  syscall(0x4652, &ret, 0, 0);
  return ret;
}

int fread(char *buffer, file f, int sector) {
  syscall(0x4652, buffer, f, &sector);
  return sector;
}

file ffind(file dir, char *fname) {
  file ret;
  syscall(0x4652, dir, fname, &ret);
  return ret;
}

void fgetwd(char *buffer) {
  syscall(0x4677, buffer);
}

void fsetwd(char *buffer) {
  syscall(0x4657, buffer);
}

void fexpand(char *dest, char *src) {
  char buffer[512];
  int ind = 0;
  int nextslash;
  int sl;
  int len = slength(src);

  nextslash = sfindc(src, '/');
  if (nextslash != 0) {
    fgetwd(dest);
  }
  else {
    ind = 1;
  }
  
  while (ind < len) {
    nextslash = sfindc(src + ind, '/');
    if (nextslash < 0)
      nextslash = len - ind;
    
    scopy(buffer, src + ind, nextslash);
    if (!scompare(buffer, ".")) {
      /* Do nothing */
    }
    else if (!scompare(buffer, "..")) {
      sl = sfindlc(dest, '/');
      if (sl < 0) {
        dest[0] = '/';
        dest[1] = '\0';
      }
      else {
        strunc(dest, sl);
      }
    }
    else {
      sconcat(dest, buffer);
      sconcat(dest, "/");
    }
    
    ind = nextslash + 1;
  }
  
  dest[slength(dest) - 1] = '\0';
}

file fget(char *path) {
  char epath[512];
  char buffer[512];
  int ind = 1;
  int nextslash;
  int len;
  file f = froot();
  fexpand(epath, path);
  len = slen(epath);
  while (ind < len) {
    nextslash = sfindc(epath + ind, '/');
    if (nextslash < 0)
      nextslash = len - ind;
    
    scopy(buffer, epath + ind, nextslash);
    f = ffind(f, buffer);
    if (f == -1)
      return f;
    
    ind = nextslash + 1;
  }
  
  return f;
}