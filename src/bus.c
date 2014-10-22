#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <nanomsg/nn.h>
#include <nanomsg/bus.h>

#define MAXEVENTS 1024

int node (const int argc, const char **argv)
{
	int sock    = nn_socket (AF_SP, NN_BUS);
	int sock_fd;
	int efd;
	struct epoll_event event;
	struct epoll_event *events;
	size_t sz = sizeof(sock_fd);
	nn_getsockopt(sock, NN_SOL_SOCKET, NN_RCVFD, &sock_fd, &sz);
	assert(sock >= 0);
	assert(nn_bind (sock, argv[2]) >= 0);
	sleep (1); // wait for connections
	if (argc >= 3) {
		int x;
		for(x = 3; x<argc; x++)
			assert (nn_connect (sock, argv[x]) >= 0);
	}
	efd = epoll_create1 (0);
	/* if (efd == -1) {
	 *	perror ("epoll_create");
	 *	abort ();
	 * }
	 */
	assert(efd != -1);

	event.data.fd = sock_fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event);
	// assert(s != -1);

	/* Buffer where events are returned */
	events = calloc (MAXEVENTS, sizeof event);
	sleep (1); // wait for connections
	int to = 100;
	assert (nn_setsockopt (sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof (to)) >= 0);
	// SEND
	int sz_n = strlen(argv[1]) + 1; // '\0' too
	printf ("%s: SENDING '%s' ONTO BUS\n", argv[1], argv[1]);
	int send = nn_send (sock, argv[1], sz_n, 0);
	assert (send == sz_n);
	while (1) {
		// RECV
		char *buf = NULL;
		int recv = nn_recv (sock, &buf, NN_MSG, 0);
		if (recv >= 0) {
			printf ("%s: RECEIVED '%s' FROM BUS\n", argv[1], buf);
			nn_freemsg (buf);
		}
	}
	return nn_shutdown (sock, 0);
}

int main (const int argc, const char **argv)
{
	if (argc >= 3)
		return node (argc, argv);
	else {
		fprintf (stderr, "Usage: bus <NODE_NAME> <URL> <URL> ...\n");
		return 1;
	}
}
