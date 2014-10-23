#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define STR_BUF 30

char* date ()
{
	struct tm *tmp = malloc(sizeof(struct tm));
	char *ttime = malloc(STR_BUF);
	time_t t = time(NULL);
	tmp = localtime(&t);
	strftime( ttime, STR_BUF, "%Y%m%d %X", tmp);
	return ttime;
}

int main (const int argc, const char **argv)
{
	printf("The date is: %s\n", date());
	return 0;
}
