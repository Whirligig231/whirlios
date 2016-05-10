/*
  Prints a string to the screen. No newline is appended.
  
  Note that any internal instances of \n will be translated into \r\n. This is done for ease of use with Unix-style line endings. To get around this behavior, you will have to send \n separately through iwritec.

ARGUMENTS
  str: the string to print
*/

void iwrites(char *str);

/*
  Prints a single character to the screen, unaltered.

ARGUMENTS
  ch: the character to print
*/

void iwritec(char ch);

/*
  Reads in a string from user input, prompting for a line of input. No newline will be appended, but a newline will be printed on the screen. The string will be given a null terminator.
  
  Backspace, left, and right are supported, but no other key is currently supported.
  
  This will also print out the string as the user types it.

ARGUMENTS
  str: a buffer in which to put the line
*/

void ireads(char *str);

/*
  Blocks and waits for a single character of input. Returns 0 if the user presses a non-ASCII key such as the arrow keys or ESC.

RETURNS
  the ASCII code of the character typed
*/

char ireadc();

/*
  Blocks and waits for a single key [combination] of input.

RETURNS
  the 16-bit keycode of the character typed
*/

int ireadk();