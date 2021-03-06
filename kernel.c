void runkernel();

/* These arrays store information about each process and are indexed by segment number */
/* Note that segment 0 is reserved for the BIOS and 1 for the kernel */
/* As such, valid segment offsets are 0x2000 through 0xf000 */
/* These are encoded as 0-13 in the process table */

int procActive[14];
char procWd[14][512];
int currentProc;
char col;

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

void setColor(char co) {
  setKernelDataSegment();
  col = co;
  restoreDataSegment();
}

void setPosition(char row, char col) {
  interrupt(0x10, 0x0200, 0, 0, (row << 8) + col);
}

void printChar(char c) {
  setKernelDataSegment();
  if (c != '\r' && c != '\n' && c != 8)
    interrupt(0x10, 0x0900 + c, col, 1, 0); 
  interrupt(0x10, 0x0e00 + c, 0, 0, 0);
  restoreDataSegment();
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

void readString(char *str, int protect, int len) {
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
    
    if (i < len-1) {
      str[i] = ch;
      printChar(protect ? '*' : ch); 
      i++;
    }
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

void writeSector(char *buffer, int sector) {
  interrupt(0x13, 0x0301, buffer, ((div(sector, 36) << 8) | mod(sector, 18)) + 1, (div(sector, 18) & 1) << 8);
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

int findFreeSector() {
  /* Note that this will also mark the sector as non-free in the SUT */
  char sut[512];
  int i;
  int byte, bit;
  readSector(sut, 1);
  for (i = 0; i < 4096; i++) {
    byte = i >> 3;
    bit = 7 - (i & 0x7);
    if (!(sut[byte] & (1 << bit))) {
      sut[byte] |= 1 << bit;
      writeSector(sut, 1);
      return i;
    }
  }
  return -1;
}

int filePutSector(char *buffer, int file, int sector) {
  int indexFlags;
  char buf[512];
  char buf2[512];
  int i;
  int index;
  int newSector;
  readSector(buf, file);
  indexFlags = ((buf[0] & 0x80) >> 6) + ((buf[1] & 0x80) >> 7);
  if (indexFlags == 0) {
    /* Unindexed file */
    if (sector > 1)
      return 1; /* EOF */
    if (sector == 0 && !buffer[504] && !buffer[505] && !buffer[506] && !buffer[507]
        && !buffer[508] && !buffer[509] && !buffer[510] && !buffer[511]) {
      /* Can be kept unindexed */
      for (i = 0; i < 504; i++)
        buf[i + 8] = buffer[i];
      writeSector(buf, file);
    }
    else {
      /* "Sector fault": convert to singly indexed and try again */
      newSector = findFreeSector();
      for (i = 0; i < 504; i++)
        buf2[i] = buf[i+8];
      for (i = 504; i < 512; i++)
        buf2[i] = 0;
      buf[1] |= 0x80;
      buf[8] = newSector >> 4;
      buf[9] = (newSector & 0xF) << 4;
      for (i = 10; i < 512; i++)
        buf[i] = 0;
      writeSector(buf, file);
      writeSector(buf2, newSector);
      return filePutSector(buffer, file, sector);
    }
  }
  else if (indexFlags == 1) {
    /* Singly indexed file */
    if (sector < 0 || sector >= 336)
      return 2; /* Would need doubly indexed */
    /* Check if the previous sector exists */
    if ((sector - 1) & 1) {
      index = ((buf[9+3*((sector - 1) >> 1)] & 0x0F) << 8) + buf[10+3*((sector - 1) >> 1)];
    } else {
      index = (buf[8+3*((sector - 1) >> 1)] << 4) + ((buf[9+3*((sector - 1) >> 1)] & 0xF0) >> 4);
    }
    if (!index)
      return 1; /* EOF */
    /* Now check the current sector */
    if ((sector) & 1) {
      index = ((buf[9+3*(sector >> 1)] & 0x0F) << 8) + buf[10+3*(sector >> 1)];
    } else {
      index = (buf[8+3*(sector >> 1)] << 4) + ((buf[9+3*(sector >> 1)] & 0xF0) >> 4);
    }
    if (!index) {
      /* Add an entry to the indexing table */
      index = findFreeSector();
      if ((sector) & 1) {
        buf[9+3*(sector >> 1)] |= index >> 8;
        buf[10+3*(sector >> 1)] = index & 0xFF;
      } else {
        buf[8+3*(sector >> 1)] = index >> 4;
        buf[9+3*(sector >> 1)] |= (index & 0xF) << 4;
      }
    }
    writeSector(buffer, index);
  }
  else {
    return 2; /* Unsupported indexing mode */
  }
  return 0;
}

int createFile(char *fname, int type) {
  char buf[512];
  int i;
  int f = findFreeSector();
  for (i = 0; i < 512; i++)
    buf[i] = 0;
  for (i = 0; i < 8 && fname[i]; i++)
    buf[i] = fname[i] & 0x7F;
  buf[2] |= (type & 0x2) << 6;
  buf[3] |= (type & 0x1) << 7;
  writeSector(buf, f);
  return f;
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

int addToDirectory(int directory, int f) {
  char buf[512];
  char name[9];
  int sector = -1;
  int i = 510, j;
  int errcode;
  int current;
  for (j = 0; j < 9; j++)
    name[j] = '\0';
  getFname(name, f);
  if (findInDirectory(directory, name))
    return 3; /* File name conflict */
  do {
    i += 2;
    if (!(i & 0x1FF)) {
      i = 0;
      sector++;
      errcode = fileGetSector(buf, directory, sector);
      if (errcode == 2)
        return 2;
      else if (errcode == 1) {
        for (j = 0; j < 512; j++)
          buf[j] = 0;
      }
    }
    current = buf[i] + (buf[i+1] << 8);
  } while (current);
  buf[i] = f & 0xFF;
  buf[i+1] = f >> 8;
  return filePutSector(buf, directory, sector);
}

int findInDirectory(int directory, char *name) {
  /* TODO: Extend this to directories with index mode 1 */
  /* TODO: Actually check that it's a directory */
  char name2[9];
  char buf[512];
  char first[512];
  int i;
  int offset;
  int index;
  int found;
  for (i = 0; i < 8 && name[i]; i++)
    name2[i] = name[i];
  for (; i < 9; i++)
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
  int segment;
  
  setKernelDataSegment();
  
  for (segment = 0; procActive[segment] && segment < 14; segment++);

  restoreDataSegment();
  
  if (segment >= 14) {
    return 1;
  }
  
  /* Copy the working directory from the launching process */
  
  setKernelDataSegment();
  
  for (i = 0; i < 512; i++) {
    procWd[segment][i] = procWd[currentProc][i];
  }
  
  restoreDataSegment();

  for (sector = 0; sector < 31; sector++) {
    /* TODO: error checking on this call */
    if (fileGetSector(buffer, file, sector))
      break;

    for (i = 0; i < 512; i++) {
      putInMemory(0x2000 + 0x1000*segment, sector*512 + i, buffer[i]);
    }

  }
  
  setKernelDataSegment();
  
  procActive[segment] = 1;
  procActive[currentProc] = 0;
  currentProc = segment;
  
  restoreDataSegment();
  
  launchProgram(0x2000 + 0x1000*segment);
  return 0; /* Included for forward compatibility once multiprocessing is a thing */
}

void terminate() {
  /* For now, relaunch the shell */
  int sec;
  int i;
  char dirbuf[4];
  
  setKernelDataSegment();
  
  for (i = 0; i < 512; i++) {
    procWd[currentProc][i] = procWd[0][i];
  }
  
  restoreDataSegment();

  sec = getRoot();

  dirbuf[0] = 'b';
  dirbuf[1] = 'i';
  dirbuf[2] = 'n';
  dirbuf[3] = '\0';
  sec = findInDirectory(sec, dirbuf);
  if (!sec) {
    setKernelDataSegment();
    printString("Error: no bin directory!");
    restoreDataSegment();
    return;
  }

  dirbuf[0] = 'w';
  dirbuf[1] = 's';
  dirbuf[2] = 'h';
  dirbuf[3] = '\0';
  sec = findInDirectory(sec, dirbuf);
  if (!sec) {
    setKernelDataSegment();
    printString("Error: no shell executable!");
    restoreDataSegment();
    return;
  }

  setKernelDataSegment();
  procActive[currentProc] = 0;
  currentProc = 0;
  restoreDataSegment();
  
  runProgram(sec);
}

void setWD(char *buffer) {
  int i;
  char c;
  for (i = 0; buffer[i] && i < 511; i++) {
    c = buffer[i];
    setKernelDataSegment();
    procWd[currentProc][i] = c;
    restoreDataSegment();
  }
  setKernelDataSegment();
  if (procWd[currentProc][i-1] != '/') {
    procWd[currentProc][i] = '/';
    i++;
  }
  procWd[currentProc][i] = '\0';
  restoreDataSegment();
}

void getWD(char *buffer) {
  int i;
  char c;
  setKernelDataSegment();
  for (i = 0; procWd[currentProc][i] && i < 511; i++) {
    c = procWd[currentProc][i];
    restoreDataSegment();
    buffer[i] = c;
    setKernelDataSegment();
  }
  restoreDataSegment();
  buffer[i] = '\0';
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
  int tmp;
  if (ax == 0x4953) {
    /* Print string */
    printString((char*) bx);
  } else if (ax == 0x4943) {
    /* Print char */
    printChar((char) bx);
  } else if (ax == 0x494B) {
    /* Set color */
    setColor((char) bx);
  } else if (ax == 0x4950) {
    /* Set position */
    setPosition((char) bx, (char) cx);
  } else if (ax == 0x4973) {
    /* Read string */
    readString((char*) bx, cx, dx);
  } else if (ax == 0x4963) {
    /* Read char */
    *((char*) bx) = readChar();
  } else if (ax == 0x496B) {
    /* Read key */
    *((int*) bx) = readKey();
  } else if (ax == 0x4958) {
    /* Execute program */
    *((int*)cx) = runProgram(bx);
  } else if (ax == 0x4978) {
    /* Exit program */
    terminate();
  } else if (ax == 0x4952) {
    /* Reboot system */
    reboot();
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
  } else if (ax == 0x4643) {
    /* Create file */
    tmp = createFile((char*) cx, *((int*) dx));
    *((int*) dx) = addToDirectory(bx, tmp);
  } else if (ax == 0x4673) {
    /* Write file sector */
    *((int*) dx) = filePutSector((char*) bx, cx, *((int*) dx));
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
  int i;
  int f;
  
  col = 7;
  
  sec = getRoot();
  /* Test making a file! */
  f = createFile("foobar", 0);
  filePutSector("I like trains.", f, 0);
  addToDirectory(sec, f);
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

  printString("Welcome to WhirliOS.\nThis is a 16-bit OS designed by Whirligig231.\n\n");
  
  makeInterrupt21();
  
  for (i = 0; i < 14; i++) {
    procActive[i] = 0;
  }
  
  procWd[0][0] = '/';
  procWd[0][1] = '\0';
  currentProc = 0;
  runProgram(sec);
}