#include "slib.h" 
 
int scompare(char str1[], char str2[]) {
	int length1 = slength(str1);
	int length2 = slength(str2);
	int pos = 0;
	int ret = 0;

	if(length1 == length2){

		while(pos <= length1){
			if(str1[pos] == str2[pos]){
				ret = 0;
			}
			pos++;
		}
	}
	else if(length1 != length2){
		ret = 1;
	}

	return(ret);
}

int slength(char str[]) {
	char *s;
  	for (s = str; *s; ++s) {}
  	return(s - str);
}

void sformati(char *buffer, int n) {
	int pos = 0;
	int n_temp;

	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
	}

	if (n < 0){
		n = -n;
		buffer[0] = '-';
		pos++;
	}

	n_temp = n;
	while (n_temp > 0) {
		n_temp = mdiv(n_temp, 10);
		pos++;
	}

	buffer[pos] = '\0';
	while (n > 0) {
		pos--;
		buffer[pos] = '0' + mmod(n, 10);
		n = mdiv(n, 10);
	}
}
