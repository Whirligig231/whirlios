/*
  Prints a string to the screen. No newline is appended.
  
  Note that any internal instances of \n will be translated into \r\n. This is done for ease of use with Unix-style line endings. To get around this behavior, you will have to send \n separately through iwritec.

ARGUMENTS
  str: the string to print
  
*/

void iwrites(char *str);