#include "flib.h"
#include "slib.h"

void finitcache(filecache *c) {
  c->sectorNumber = -1;
}

file froot() {
  int ret;
  syscall(0x4652, &ret, 0, 0);
  return ret;
}

int freads(char *buffer, file f, int sector) {
  syscall(0x4653, buffer, f, &sector);
  return sector; /* reuse input parameter as output parameter */
}

int freadb(file f, int byte, filecache *c) {
  int sector, offset, errcode;
  char buffer[512];

  sector = (byte >> 9);
  offset = (byte & 0x1FF);
  if (c == 0) {
    /* uncached mode */
    errcode = freads(buffer, f, sector);
    if (errcode == 1)
      return F_EOF;
    if (errcode == 2)
      return F_ERR;
    return buffer[offset];
  }
  if (sector != c->sectorNumber) {
    c->sectorNumber = sector;
    errcode = freads(c->sectorData, f, sector);
    if (errcode == 1) {
      /* end of file */
      c->sectorNumber = -1; /* invalidate cache */
      return F_EOF;
    }
    if (errcode == 2) {
      /* error in read */
      c->sectorNumber = -1; /* invalidate cache */
      return F_ERR;
    }
  }
    
  return c->sectorData[offset];
}

file ffind(file dir, char *fname) {
  file ret;
  syscall(0x4646, dir, fname, &ret);
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
    dest[0] = '/';
    dest[1] = '\0';
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
    
    ind += nextslash + 1;
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
  
  len = slength(epath);
  while (ind < len) {
    nextslash = sfindc(epath + ind, '/');
    if (nextslash < 0)
      nextslash = len - ind;
    
    scopy(buffer, epath + ind, nextslash);
    f = ffind(f, buffer);
    if (f == 0)
      return f;
    
    ind += nextslash + 1;
  }

  return f;
}

void fgetname(char *buffer, file f) {
  syscall(0x466E, buffer, f, 0);
}

int fgettype(file f) {
  int ret;
  syscall(0x4674, f, &ret, 0);
  return ret;
}

file fdirget(file dir, int index, filecache *c) {
  int byte;
  file retvalue;
  byte = freadb(dir, 2*index, c);
  if (byte == F_EOF || byte == F_ERR)
    return 0;
  retvalue = byte;
  byte = freadb(dir, 2*index + 1, c);
  if (byte == F_EOF || byte == F_ERR)
    return 0;
  retvalue |= (byte << 8);
  return retvalue;
}