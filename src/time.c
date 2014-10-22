#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

char* date ()
{
	struct tm *tmp = malloc(sizeof(struct tm));
	char *ttime = malloc(30);
	time_t t = time(NULL);
	tmp = localtime(&t);
	strftime( ttime, 30, "%Y%m%d %X", tmp);
	printf("Time: %s\n", ttime);
	return ttime;
}

int main (const int argc, const char **argv)
{
	printf("The date is: %s\n", date());
	return 0;
}
