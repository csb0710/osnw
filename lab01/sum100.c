#include <stdio.h>

void sum100() {
	int i;
	int sum = 0;

	for (i = 1; i < 101; i++) {
		sum += i; 
	}
	
	printf("%d\n", sum);
}
