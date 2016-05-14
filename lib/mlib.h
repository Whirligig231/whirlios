/*
  Computes a/b, rounding down. Note that this is *not* C99's division--rather than truncating toward zero, this always truncates downward. For instance, mdiv(-7, 2) == -4.
  
  In the case where b < 0, mdiv(a, b) == mdiv(-a, -b). If b == 0, mdiv(a, b) == 0.

ARGUMENTS
  a, b: integers to divide
*/

int mdiv(int a, int b);

/*
  Computes a%b, rounding down. Note that this is *not* C99's modulo--in particular, mmod(a, b) will always be nonnegative. Thus, mdiv(a, b)*b + mmod(a, b) == a.
  
  In the case where b == 0, mmod(a, b) == a.

ARGUMENTS
  a, b: integers to compute a%b on
*/

int mmod(int a, int b);

/*
  This struct represents a long (32-bit) integer. Since ld86 does not support 32-bit instructions, this functionality is provided instead.
  
FIELDS
  low: the low 16 bits
  high: the high 16 bits
*/

typedef struct lint {
  unsigned int low;
  unsigned int high;
} lint;

/*
  Computes a+b, where a, b are lints.

ARGUMENTS
  a, b: long integers to add
*/

lint maddl(lint a, lint b);