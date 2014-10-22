#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <nanomsg/nn.h>
#include <nanomsg/survey.h>

#define SERVER "server"
#define CLIENT "client"
#define DATE   "DATE"

char* date ()
{
	struct tm *tmp = malloc(sizeof(struct tm));
	char *ttime = malloc(30);
	time_t t = time(NULL);
	tmp = localtime(&t);
	strftime( ttime, 30, "%Y%m%d %X", tmp);
	return ttime;
}

int server (const char *url)
{
	int sock = nn_socket (AF_SP, NN_SURVEYOR);
	assert (sock >= 0);
	assert (nn_bind (sock, url) >= 0);
	sleep(1); // wait for connections
	printf ("SERVER: SENDING DATE SURVEY REQUEST TIME[%s]\n", date());
	int bytes = nn_send (sock, DATE, sizeof(DATE), 0);
	// assert (bytes == sizeof(DATE));
	while (1) {
		char *buf = NULL; // malloc(1024);
		int bytes = nn_recv (sock, &buf, NN_MSG, 0);
		// if (bytes == ETIMEDOUT) break;
		if (bytes >= 0) {
			printf ("SERVER: RECEIVED \"%s\" SURVEY RESPONSE\n", buf);
			nn_freemsg (buf);
		}
	}
	return nn_shutdown (sock, 0);
}

int client (const char *url, const char *name)
{
	int sock = nn_socket (AF_SP, NN_RESPONDENT);
	assert (sock >= 0);
	assert (nn_connect (sock, url) >= 0);
	while (1) {
		char *buf = malloc(1024);
		int bytes = nn_recv (sock, &buf, NN_MSG, 0);
		if (bytes >= 0) {
			int i;
				printf ("CLIENT (%s): RECEIVED \"%s\" SURVEY REQUEST\n", name, buf);
				nn_freemsg (buf);
			for (i =0; i<3;++i) {
				printf ("CLIENT (%s): SENDING DATE SURVEY RESPONSE\n", name);
				char *msg = malloc(50);
				sprintf(msg, "client[%s] date[%s]", name, date());
				int bytes = nn_send (sock, msg, 50, 0);
				sleep(1);
			}
			// assert (bytes == sizeof(d));
		}
	}
	return nn_shutdown (sock, 0);
}

int main (const int argc, const char **argv)
{
	if (strncmp (SERVER, argv[1], strlen (SERVER)) == 0 && argc >= 2)
		return server (argv[2]);
	else if (strncmp (CLIENT, argv[1], strlen (CLIENT)) == 0 && argc >= 3)
		return client (argv[2], argv[3]);
	else {
		fprintf (stderr, "Usage: survey %s|%s <URL> <ARG> ...\n",
			SERVER, CLIENT);
		return 1;
	}
}
