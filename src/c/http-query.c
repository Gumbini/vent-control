#include "http-query.h"

#include <netdb.h>		 // getaddrinfo()
#include <sys/socket.h>	 // socket()
#include <sys/types.h>	 // socket()
#include <unistd.h>		 // dup()

#include "utility.h"

int connectTo(const char *restrict hostname, const char *restrict port) {
	struct addrinfo hints = {
		.ai_socktype = SOCK_STREAM,	 // TCP
		.ai_family = AF_UNSPEC,		 // Unspecified address familiy
		.ai_flags = AI_ADDRCONFIG,	 // Only locally available address types
	};

	struct addrinfo *head;
	int gai_result;
	if ((gai_result = getaddrinfo(hostname, port, &hints, &head))) {
		if (gai_result == EAI_SYSTEM)
			die("getaddrinfo");
		die(gai_strerror(gai_result));
	}

	int sock;
	struct addrinfo *curr;
	for (curr = head; curr != NULL; curr = curr->ai_next) {	 // Try connecting to the server
		if ((sock = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1)
			continue;
		if (!connect(sock, curr->ai_addr, curr->ai_addrlen))
			break;
		close(sock);
	}

	if (sock == -1)
		die("socket");

	if (curr == NULL)  // No usable address found
		die("connect");

	freeaddrinfo(head);	 // Free memory allocated by getaddrinfo()

	return sock;
}

void initFileDescriptors(FILE **rx, FILE **tx, int sock) {
	int sock_copy = dup(sock);	// Duplicate the file descriptor to avoid buffering issues
	if (sock_copy == -1)
		die("dup");

	*rx = fdopen(sock, "r");
	if (!*rx)
		die("fdopen");

	*tx = fdopen(sock_copy, "w");
	if (!*tx)
		die("fdopen");
}
