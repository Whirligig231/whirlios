#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct sector_t {
  int index;
  char data[512];
} sector_t;

int sectorUsed[4096];
sector_t sectors[1000];
int totalSectors;

sector_t writeSUT() {
  sector_t ret;
  ret.index = 1;
  for (int i = 0; i < 512; i++) {
    ret.data[i] = 0;
    for (int j = 0; j < 8; j++) {
      ret.data[i] += sectorUsed[i*8+j] << (7 - j);
    }
  }
  return ret;
}

void appendSector(sector_t sec) {
  sectorUsed[sec.index] = 1;
  sectors[totalSectors] = sec;

  char name[9];
  strncpy(name, sec.data, 8);
  name[8] = '\0';
  for (int i = 0; i < 8; i++)
    name[i] &= 0x7F;
  printf("Writing sector \"%s\" to slot %d ...\n", name, sec.index);
  totalSectors++;
}

char *getSector(int index) {
  for (int i = 0; i < 1000; i++) {
    if (sectors[i].index == index)
      return sectors[i].data;
  }
  return NULL;
}

int writeSector(char *buffer) {
  int index = 16;
  while (index < 4096 && sectorUsed[index])
    index++;
  if (index == 4096)
    return -1; // should never happen with current limits
  sector_t sec;
  sec.index = index;
  for (int i = 0; i < 512; i++)
    sec.data[i] = buffer[i];
  appendSector(sec);
  return index;
}

int writeFile(char *dest, char *src, int type) {
  char buf[512];
  for (int i = 0; i < 512; i++)
    buf[i] = 0;
  strncpy(buf, dest, 8);

  buf[2] |= 0x80 & (type << 6);
  buf[3] |= 0x80 & (type << 7);

  FILE *f = fopen(src, "r");
  if (!f) {
    printf("Error: file %s not found!\n", src);
    return -1;
  }
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  if (size <= 504) {
    // Unindexed file
    rewind(f);
    int c;
    int i = 8;
    while ((c = fgetc(f)) != EOF) {
      buf[i] = c;
      i++;
    }

    fclose(f);
    return writeSector(buf);
  }
  else {
    // Indexed file
    buf[1] |= 0x80;
    rewind(f);
    int sectorInd = 0;
    char sect[512];
    int c;
    int i = 0;
    for (int j = 0; j < 512; j++)
      sect[j] = 0;
    while ((c = fgetc(f)) != EOF) {
      sect[i] = c;
      i++;
      if (i == 512) {
        i = 0;
        int index = writeSector(sect);
        int offset = 8 + 3*(sectorInd >> 1);
        if (sectorInd & 1) {
          buf[offset+1] += (index >> 8) << 4;
          buf[offset+2] = index & 0xFF;
        }
        else {
          buf[offset] = index >> 4;
          buf[offset+1] += (index & 0xF) << 4;
        }
        sectorInd++;
        for (int j = 0; j < 512; j++)
          sect[j] = 0;
      }
    }
    int index = writeSector(sect);
    int offset = 8 + 3*(sectorInd >> 1);
    if (sectorInd & 1) {
      buf[offset+1] += (index >> 8) << 4;
      buf[offset+2] = index & 0xFF;
    }
    else {
      buf[offset] = index >> 4;
      buf[offset+1] += (index & 0xF) << 4;
    }

    fclose(f);
    return writeSector(buf);
  }
}

int writeDirectory(char *name) {
  char buf[512];
  for (int i = 0; i < 512; i++)
    buf[i] = 0;
  strncpy(buf, name, 8);
  buf[2] |= 0x80;
  buf[3] |= 0x80;

  return writeSector(buf);
}

int addToDirectory(int index, int fileInd) {
  char *sector = getSector(index);
  int i = 8;
  while (sector[i] || sector[i+1])
    i += 2;
  printf("Adding index %d at offset %d in sector %d ...\n", fileInd, i, index);
  sector[i+1] = fileInd >> 8;
  sector[i] = fileInd & 0xFF;
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < 4096; i++)
    sectorUsed[i] = 0;
  for (int i = 0; i < 16; i++)
    sectorUsed[i] = 1;
  totalSectors = 0;

  int root = writeDirectory("");
  int bin = writeDirectory("bin");
  addToDirectory(root, bin);
  int wsh = writeFile("wsh", "wsh.e", 2);
  addToDirectory(bin, wsh);
  int hello = writeFile("hello", "programs/hello.e", 2);
  addToDirectory(bin, hello);
  int calc = writeFile("calc", "programs/calc.e", 2);
  addToDirectory(bin, calc);
  int test = writeFile("test", "test.txt", 0);
  addToDirectory(root, test);
  int blah = writeDirectory("blah");
  addToDirectory(root, blah);
  int hello2 = writeFile("hello", "programs/hello2.e", 2);
  addToDirectory(blah, hello2);

  appendSector(writeSUT());

  FILE *f = fopen("whirlios.img", "rb");
  fseek(f, 0, SEEK_END);
  int len = ftell(f);
  rewind(f);

  for (int i = 0; i < totalSectors; i++) {
    if (sectors[i].index * 512 + 512 > len)
      len = sectors[i].index + 512;
  }

  char *floppy = malloc(len);
  for (int i = 0; i < len; i++)
    floppy[i] = 0;
  fread(floppy, 1, len, f);
  fclose(f);

  for (int i = 0; i < totalSectors; i++) {
    int base = sectors[i].index * 512;
    memcpy(floppy + base, sectors[i].data, 512);
  }

  f = fopen("whirlios.img", "wb");
  fwrite(floppy, 1, len, f);
  fclose(f);

  free(floppy);
  return 0;
}
