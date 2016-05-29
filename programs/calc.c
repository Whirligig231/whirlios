#include "../lib/mlib.h"
#include "../lib/ilib.h"
#include "../lib/slib.h"

int main() {
char op[4];
char num1[7];
int num11 = 0;
char num2[7];
int num22 = 0;
int len1;
int len2;
int i;
int ans;
char str[7];

iwrites("add, sub, div, or mul?\n");
ireads(op, 4);
iwrites("\nfirst number?\n");
ireads(num1, 7);
iwrites("\nsecond number?\n");
ireads(num2, 7);
iwrites("\n");

len1 = slength(num1);
len2 = slength(num2);

for(i = 0; i < len1;i++) {
  num11 = num11 * 10 + ( num1[i] - '0' );
}
for(i = 0; i < len2;i++) {
  num22 = num22 * 10 + ( num2[i] - '0' );
}

if(len1 == 0) {
  num11 = 0;
}
if(len2 == 0) {
  num22 = 0;
}

if(!scompare(op, "add")) {
  ans = num11 + num22;
}
else if(!scompare(op, "sub")) {
  ans = num11 - num22;
}
else if(!scompare(op, "mul")) {
  ans = num11 * num22;
}
else if(!scompare(op, "div")) {
  ans = mdiv(num11, num22);
}
else {
  iwrites("try again\n");
  iexit();
}

sformati(str, ans);
iwrites(str);
iwrites("\n");

iexit();

}
