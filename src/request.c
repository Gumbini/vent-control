#include "request.h"

#include <errno.h>      // errno
#include <netdb.h>      // getaddrinfo()
#include <stdio.h>      // fdopen()
#include <stdlib.h>     // free()
#include <sys/socket.h> // socket()
#include <unistd.h>     // dup()

#include "utility.h"

static int connectTo(const char *restrict hostname, const char *restrict port) {
	struct addrinfo hints = {
		.ai_socktype = SOCK_STREAM,   // TCP
		.ai_family   = AF_UNSPEC,     // Unspecified address familiy
		.ai_flags    = AI_ADDRCONFIG, // Only locally available address types
	};

	struct addrinfo *head = NULL;
	int gai_result;
	if ((gai_result = getaddrinfo(hostname, port, &hints, &head))) {
		if (gai_result == EAI_SYSTEM)
			die("getaddrinfo");
		die(gai_strerror(gai_result));
	}

	int sock = -1;
	struct addrinfo *curr;
	for (curr = head; curr != NULL; curr = curr->ai_next) { // Try connecting to the server
		if ((sock = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1)
			continue;
		if (!connect(sock, curr->ai_addr, curr->ai_addrlen))
			break;
		close(sock);
	}

	freeaddrinfo(head); // Free memory allocated by getaddrinfo()

	if (sock == -1)
		die("socket");

	if (curr == NULL) // No usable address found
		return -1;

	return sock;
}

static void initFileDescriptors(FILE **rx, FILE **tx, int sock) {
	int sock_copy = dup(sock); // Duplicate the file descriptor to avoid buffering issues
	if (sock_copy == -1)
		die("dup");

	*rx = fdopen(sock, "r");
	if (!*rx)
		die("fdopen");

	*tx = fdopen(sock_copy, "w");
	if (!*tx)
		die("fdopen");
}

int requestApi(requestInfo_t *reqInfo, char **reqStorage) {
	FILE *rx = NULL;
	FILE *tx = NULL;
	int sock = connectTo(reqInfo->host, reqInfo->port); // Never fails
	if (sock == -1) {
		// errno set by connectTo
		return 0;
	}

	initFileDescriptors(&rx, &tx, sock);

	fprintf(tx, "GET %s HTTP/1.1" CRLF, reqInfo->request);
	fflush(tx);

	char *msg;
	while (1) {
		msg = readLineAll(rx);
		if (msg == NULL) {
			if (fclose(rx)) // Close file descriptors
				die("fclose");
			if (fclose(tx))
				die("fclose");
			fprintf(stderr, "Server responded with EOF to early!\n");
			errno = EBADE;
			return 0;
		}

		if (STRING_STARTS_WITH(msg, "HTTP/1.1 200 OK")) {
			free(msg);
			continue;
		}
		if (STRING_STARTS_WITH(msg, "Content-type:text/html")) {
			free(msg);
			continue;
		}
		if (STRING_STARTS_WITH(msg, "Connection:")) {
			free(msg);
			continue;
		}
		if (strlen(msg) == 0) {
			free(msg);
			continue;
		}

		break;
	}

	if (fclose(rx)) // Close file descriptors
		die("fclose");
	if (fclose(tx))
		die("fclose");

	if (msg[0] == '\0') {
		fprintf(stderr, "Null answer received!\n");
		errno = EBADMSG;
		return 0;
	}

	*reqStorage = msg;
	return 1;
}
