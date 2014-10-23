#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <nanomsg/nn.h>
#include <nanomsg/bus.h>

#define MAX_EVENTS 16

int node (const int argc, const char **argv)
{
	int sock    = nn_socket (AF_SP, NN_BUS);
	int sock_fd, efd, nfds;
	struct epoll_event event, events[MAX_EVENTS];
	size_t sz = sizeof(sock_fd);
	nn_getsockopt(sock, NN_SOL_SOCKET, NN_RCVFD, &sock_fd, &sz);
	assert(sock >= 0);
	assert(nn_bind (sock, argv[2]) >= 0);
	sleep (1); // wait for connections
	if (argc >= 3) {
		int x;
		for(x = 3; x < argc; x++)
			assert(nn_connect(sock, argv[x]) >= 0);
	}
	efd = epoll_create(MAX_EVENTS);
	assert(efd != -1);

	event.data.fd = sock_fd;
	// event.events = EPOLLIN | EPOLLET;
	event.events = EPOLLIN; // nanomsg is level triggered.
	assert(epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event) != -1);
	// events = malloc(MAX_EVENTS * sizeof(event));

	sleep(1); // wait for connections
	int to = 100;
	assert(nn_setsockopt (sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to)) >= 0);

	// SEND
	int sz_n = strlen(argv[1]) + 1; // '\0' too
	printf ("%s: SENDING '%s' ONTO BUS\n", argv[1], argv[1]);
	int send = nn_send (sock, argv[1], sz_n, 0);
	assert (send == sz_n);

	for(;;) {
		nfds = epoll_wait(efd, events, MAX_EVENTS, -1);
		assert(nfds != -1);

		int n, recv;
		for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd == sock_fd) {
				char *buf = NULL;
				recv = nn_recv(sock, &buf, NN_MSG, 0);
				if (recv >= 0) {
					printf ("%s: RECEIVED '%s' FROM BUS\n", argv[1], buf);
					nn_freemsg (buf);
				}
				/*
				event.events = EPOLLIN | EPOLLET;
				event.data.fd = conn_sock;
				if (epoll_ctl(efd, EPOLL_CTL_ADD, conn_sock,
					&event) == -1) {

					perror("epoll_ctl: conn_sock");
					exit(EXIT_FAILURE);
				} else {
					do_use_fd(events[n].data.fd);
				}
				*/
			}
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
