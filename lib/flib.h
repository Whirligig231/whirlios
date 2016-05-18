/*
  File reading code for end of file.
*/
#define F_EOF -1

/*
  File reading code for an unexpected file read error.
*/
#define F_ERR -2

/*
  Represents a pointer to the beginning of a file on disk.
*/
typedef int file;

/*
  This struct is used to speed file reads. When used properly, it can nearly double file reading speed.
  
  For any function that uses a filecache, pass a filecache object. Make sure to initialize it first!
  
  If you do not want to use a filecache, you may pass NULL instead.
  
FIELDS
  sectorData: the data in the least recently read sector
  sectorNumber: the index in the file of the least recently read sector
*/
typedef struct filecache {
	char sectorData[512];
	int sectorNumber;
} filecache;

/*
  Initializes a filecache.
  
ARGUMENTS
  c: the cache to initialize
*/

void finitcache(filecache *c);

/*
  Gets a pointer to the root of the file system.

RETURNS
  a pointer to the root directory
*/
file froot();

/*
  Reads a sector from a file.

ARGUMENTS
  buffer: buffer (512 B) in which to put the file data
  f: the file to read
  sector: the sector to read within this file

RETURNS
  0 if correctly read, 1 if EOF, 2 if there was another error
*/
int freads(char *buffer, file f, int sector);

/*
  Reads a single byte from a file. Note that this function only supports up to 32 KB due to the size of the byte argument.

ARGUMENTS
  f: the file to read from
  byte: the index of the byte to read
  c: a cache to use (or NULL)

RETURNS
  the byte read, or one of the file reading codes F_EOF/F_ERR
*/
int freadb(file f, int byte, filecache *c);

/*
  Finds a file with a given name in a directory. This is not recursive.

ARGUMENTS
  dir: the directory to look in
  fname: the name of the file

RETURNS
  the file with that name, or 0 if no such file was found
*/
file ffind(file dir, char *fname);

/*
  Gets the full path to the working directory.

ARGUMENTS
  buffer: a buffer into which to load the directory path
*/
void fgetwd(char *buffer);

/*
  Sets the full path to the working directory.

ARGUMENTS
  buffer: a buffer from which to load the directory path
*/
void fsetwd(char *buffer);

/*
  Expands a condensed file path (i.e. using ., .., etc.) into a full file path.

ARGUMENTS
  dest: a buffer into which to put the full path
  src: a buffer containing the condensed path
*/
void fexpand(char *dest, char *src);

/*
  Gets the file at a condensed or full path.

ARGUMENTS
  path: the path to the file, condensed or full
*/
file fget(char *path);

/*
  Gets the name of a file.

ARGUMENTS
  buffer: a buffer into which to put the name
  f: the file to get the name of
*/
void fgetname(char *buffer, file f);

/*
  Gets the type of a file. Here 0 indicates a text file, 1 indicates a binary file, 2 is an executable, and 3 is a directory.

ARGUMENTS
  f: the file to get the type of
  
RETURNS
  the file type, as above
*/
int fgettype(file f);

/*
  Returns the nth file in a directory.

ARGUMENTS
  dir: the directory to look in
  index: the index of the file in the directory
  c: a filecache to use, or NULL
  
RETURNS
  the file pointer, or 0 if there are not that many files
*/
file fdirget(file dir, int index, filecache *c);