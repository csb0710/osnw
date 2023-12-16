#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void main()
{
	time_t t;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	printf("이름 : 차승빈, time : %s", asctime(tm));
}
