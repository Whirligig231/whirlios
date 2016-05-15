/*
  Compares a string to another string. Returns 0 if the strings are equal, -1 if the second follows the first in ASCII order, and 1 if the first follows the second.

ARGUMENTS
  str1: the first string
  str2: the second string

RETURNS
  0, 1, or -1; see above
*/

int scompare(char *str1, char *str2);

/*
  Finds the length of a string.

ARGUMENTS
  str: the string

RETURNS
  the length of the string
*/

int slength(char *str);

/*
  Copies the first len characters of src to dest. Useful for extracting substrings.
  
  The output buffer must be long enough to hold the new string; there is no protection for this. A null terminator will be appended after the copied string.

ARGUMENTS
  dest: output buffer
  src: input buffer
  len: maximum length to copy
*/
void scopy(char *dest, char *src, int len);

/*
  Concatenates two strings. This assumes the dest buffer is large enough, so be careful.
  
ARGUMENTS
  dest: output buffer
  src: input buffer
*/
void sconcat(char *dest, char *src);

/*
  Truncates a string, removing all but the first len characters. This is mutative.
  
ARGUMENTS
  str: the buffer
  len: the new length of the string
*/
void strunc(char *str, int len);

/*
  Finds a character in a string. Returns the offset of the first occurrence.
  
ARGUMENTS
  str: the string in which to search
  ch: the character to find
  
RETURNS
  the offset of ch in str, or -1 if it was not found
*/
int sfindc(char *str, char ch);

/*
  Finds one string in another. Returns the offset of the first occurrence.
    
ARGUMENTS
  str1: the string in which to search
  str2: the string to find
  
RETURNS
  the offset of str2 in str1, or -1 if it was not found
*/
int sfinds(char *str1, char *str2);

/*
  Finds a character in a string. Returns the offset of the last occurrence.
  
ARGUMENTS
  str: the string in which to search
  ch: the character to find
  
RETURNS
  the offset of the last occurrence of ch in str, or -1 if it was not found
*/
int sfindlc(char *str, char ch);

/*
  Formats an integer to a string. Note that this assumes the buffer is large enough (7 bytes).

ARGUMENTS
  n: integer to be converted
  buffer: string to output to
*/

void sformati(char *buffer, int n);
