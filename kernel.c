void runkernel();

int main() {
	runkernel();
  while (1) continue;
}

int div(int a, int b) {
  int quot;
  int q;
  int c;
  quot = 0;
  while (a >= b) {
    c = b;
    q = 1;
    do {
      c <<= 1;
      q <<= 1;
    } while (c <= a && c < 8192);
    c >>= 1;
    q >>= 1;
    a -= c;
    quot += q;
  }
  return quot;
}

int mod(int a, int b) {
  int c;
  while (a >= b) {
    c = b;
    do {
      c <<= 1;
    } while (c <= a && c < 8192);
    c >>= 1;
    a -= c;
  }
  return a;
}

void printChar(char c) {
  interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void printString(char *str) {
  int i;
  for (i = 0; str[i]; i++) {
    if (str[i] == '\n')
      printChar('\r');
    printChar(str[i]);
  }
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x0201, buffer, ((div(sector, 36) << 8) | mod(sector, 18)) + 1, (div(sector, 18) & 1) << 8);
}

int fileGetSector(char *buffer, int file, int *sector) {
  /* TODO: Make this not suck as much (currently it does two reads sometimes, which could theoretically be helped) */
  int indexFlags;
  char buf[512];
  int i;
  int index;
  readSector(buf, file);
  indexFlags = ((buf[0] & 0x80) >> 6) + ((buf[1] & 0x80) >> 7);
  if (indexFlags == 0) {
    /* Unindexed file */
    if (*sector)
      return 1; /* EOF */
    for (i = 0; i < 504; i++) {
      buffer[i] = buf[i+8];
    }
    for (; i < 512; i++)
      buffer[i] = 0;
  } else if (indexFlags == 1) {
    /* Singly indexed file */
    if (*sector < 0 || *sector >= 336)
      return 1; /* EOF */
    if ((*sector) & 1) {
      index = ((buf[9+3*(*sector >> 1)] & 0x0F) << 8) + buf[10+3*(*sector >> 1)];
    } else {
      index = (buf[8+3*(*sector >> 1)] << 4) + ((buf[9+3*(*sector >> 1)] & 0xF0) >> 4);
    }
    if (!index)
      return 1; /* EOF */
    readSector(buffer, index);
  } else {
    return 2; /* Unsupported indexing mode */
  }
  (*sector)++;
  return 0;
}

int getRoot() {
  return 16;
}

int findInDirectory(int directory, char *name) {
  /* TODO: Extend this to directories with index mode 1 */
  /* TODO: Actually check that it's a directory */
  char name2[8];
  char buf[512];
  char first[512];
  int i;
  int offset;
  int index;
  int sector;
  int found;
  sector = 0;
  for (i = 0; i < 8 && name[i]; i++)
    name2[i] = name[i];
  for (; i < 8; i++)
    name2[i] = '\0';
  fileGetSector(buf, directory, &sector);
  for (offset = 0; offset < 512; offset += 2) {
    index = (buf[offset+1] << 8) + buf[offset];
    if (!index)
      break;
    readSector(first, index);
    found = 1;
    for (i = 0; i < 8; i++) {
      if ((first[i] & 0x7F) != name2[i])
        found = 0;
    }
    if (found)
      return index;
  }
  return -1;
}

int runProgram(int file) {
  int sector;
  char buffer[512];
  int i;
  
  sector = 0;
  while (sector < 32) {
    /* TODO: error checking on this call */
    if (fileGetSector(buffer, file, &sector))
      break;

    for (i = 0; i < 512; i++) {
      putInMemory(0x2000, (sector - 1)*512 + i, buffer[i]);
    }
  }
  
  launchProgram(0x2000);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
  if (ax == 0x4953) {
    /* Print string */
    printString((char*) bx);
  } else {
    /* Incorrect interrupt -- for now, do nothing */
    return;
  }
}

int handleTimerInterrupt() {
  /* Placeholder */
}

void runkernel() {
  int sec;
  sec = getRoot();
  sec = findInDirectory(sec, "bin");
  if (sec == -1) {
    printString("Error: no bin directory!");
    return;
  }
  sec = findInDirectory(sec, "wsh");
  if (sec == -1) {
    printString("Error: no shell executable!");
    return;
  }
  
  makeInterrupt21();
  runProgram(sec);
}