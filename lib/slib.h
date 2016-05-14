 /*
  Compares a string to another string.

  ARGUMENTS
    str1: The first string
    str2: The second string

  RETURNS
    0: If the two strings are equal
    1: If the two strings are not equal
*/

int scompare(char str1[], char str2[]);

/*
  Finds length of string

  ARGUMENTS
    str: The string

  RETURNS:
    len: Length of the string
*/

int slength(char str[]);

/*
  Formats an integer to a string.

  ARGUMENTS:
    n: Integer to be converted
    buffer: String to outout to
*/

void sformati(char *buffer, int n);