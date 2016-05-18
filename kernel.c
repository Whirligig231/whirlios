void runkernel();

char wd[512];

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

char readChar() {
  return interrupt(0x16, 0, 0, 0, 0) & 0xFF;
}

int readKey() {
  return interrupt(0x16, 0, 0, 0, 0);
}

void readString(char *str, int protect) {
  /* TODO: Remove buffer overflow */
  int i, max, j;
  int key;
  char ch;
  i = 0;
  max = 0;
  j = 0;
  ch = 0;
  while (ch != '\r') {
    key = readKey();
    ch = key & 0xFF;
    
    if (ch == '\r')
      break;
    
    if (ch == 8) {
      /* Backspace */
      if (i == 0)
        continue;
      i--;
      printChar(8);
      for (j = i; j < max - 1; j++) {
        str[j] = str[j+1];
        printChar(protect ? '*' : str[j]);
      }
      printChar(' ');
      for (j = max; j > i; j--) {
        printChar(8);
      }
      max--;
      continue;
    }
    
    if (ch == 0) {
      if (key >> 8 == 0x4D) {
        /* Right arrow */
        if (i < max) {
          printChar(protect ? '*' : str[i]);
          i++;
        }
      } else if (key >> 8 == 0x4B) {
        /* Left arrow */
        if (i > 0) {
          i--;
          printChar(8);
        }
      }
      continue;
    }
    
    str[i] = ch;
    printChar(protect ? '*' : ch);
    i++;
    if (i > max)
      max = i;
  }
  
  
  str[max] = '\0';
  printChar('\r');
  printChar('\n');
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x0201, buffer, ((div(sector, 36) << 8) | mod(sector, 18)) + 1, (div(sector, 18) & 1) << 8);
}

int fileGetSector(char *buffer, int file, int sector) {
  /* TODO: Make this not suck as much (currently it does two reads sometimes, which could theoretically be helped) */
  int indexFlags;
  char buf[512];
  int i;
  int index;
  readSector(buf, file);
  indexFlags = ((buf[0] & 0x80) >> 6) + ((buf[1] & 0x80) >> 7);
  if (indexFlags == 0) {
    /* Unindexed file */
    if (sector)
      return 1; /* EOF */
    for (i = 0; i < 504; i++) {
      buffer[i] = buf[i+8];
    }
    for (; i < 512; i++)
      buffer[i] = 0;
  } else if (indexFlags == 1) {
    /* Singly indexed file */
    if (sector < 0 || sector >= 336)
      return 1; /* EOF */
    if ((sector) & 1) {
      index = ((buf[9+3*(sector >> 1)] & 0x0F) << 8) + buf[10+3*(sector >> 1)];
    } else {
      index = (buf[8+3*(sector >> 1)] << 4) + ((buf[9+3*(sector >> 1)] & 0xF0) >> 4);
    }
    if (!index)
      return 1; /* EOF */
    readSector(buffer, index);
  } else {
    return 2; /* Unsupported indexing mode */
  }
  return 0;
}

void getFname(char *buffer, int file) {
  char buf[512];
  int i;
  readSector(buf, file);
  for (i = 0; i < 8 && (buf[i] & 0x7F); i++) {
    buffer[i] = buf[i] & 0x7F;
  }
  buffer[i] = '\0';
}

int getFtype(int file) {
  char buf[512];
  int i;
  readSector(buf, file);
  return ((buf[2] & 0x80) >> 6) | ((buf[3] & 0x80) >> 7);
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
  int found;
  for (i = 0; i < 8 && name[i]; i++)
    name2[i] = name[i];
  for (; i < 8; i++)
    name2[i] = '\0';
  fileGetSector(buf, directory, 0);
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
  return 0;
}

int runProgram(int file) {
  int sector;
  char buffer[512];
  int i;

  for (sector = 0; sector < 32; sector++) {
    /* TODO: error checking on this call */
    if (fileGetSector(buffer, file, sector))
      break;

    for (i = 0; i < 512; i++) {
      putInMemory(0x2000, sector*512 + i, buffer[i]);
    }

  }
  
  launchProgram(0x2000);
}

void setWD(char *buffer) {
  int i;
  char c;
  for (i = 0; buffer[i] && i < 511; i++) {
    c = buffer[i];
    setKernelDataSegment();
    wd[i] = c;
    restoreDataSegment();
  }
  setKernelDataSegment();
  wd[i] = '\0';
  restoreDataSegment();
}

void getWD(char *buffer) {
  int i;
  char c;
  setKernelDataSegment();
  for (i = 0; wd[i] && i < 511; i++) {
    c = wd[i];
    restoreDataSegment();
    buffer[i] = c;
    setKernelDataSegment();
  }
  restoreDataSegment();
  buffer[i] = '\0';
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
  if (ax == 0x4953) {
    /* Print string */
    printString((char*) bx);
  } else if (ax == 0x4943) {
    /* Print char */
    printChar((char) bx);
  } else if (ax == 0x4973) {
    /* Read string */
    readString((char*) bx, cx);
  } else if (ax == 0x4963) {
    /* Read char */
    *((char*) bx) = readChar();
  } else if (ax == 0x496B) {
    /* Read key */
    *((int*) bx) = readKey();
  } else if (ax == 0x4653) {
    /* Read file sector */
    *((int*) dx) = fileGetSector((char*) bx, cx, *((int*) dx));
  } else if (ax == 0x4646) {
    /* Find in directory */
    *((int*) dx) = findInDirectory(bx, (char*) cx);
  } else if (ax == 0x4652) {
    /* Get root directory */
    *((int*) bx) = getRoot();
  } else if (ax == 0x4657) {
    /* Set working directory */
    setWD((char*) bx);
  } else if (ax == 0x4677) {
    /* Get working directory */
    getWD((char*) bx);
  } else if (ax == 0x466E) {
    /* Get file name */
    getFname((char*) bx, cx);
  } else if (ax == 0x4674) {
    /* Get file type */
    *((int*)cx) = getFtype(bx);
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
  if (!sec) {
    printString("Error: no bin directory!");
    return;
  }
  sec = findInDirectory(sec, "wsh");
  if (!sec) {
    printString("Error: no shell executable!");
    return;
  }
  
  makeInterrupt21();
  wd[0] = '/';
  wd[1] = '\0';
  runProgram(sec);
}