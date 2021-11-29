#include <errno.h>		 // errno
#include <math.h>		 // std::isnan()
#include <netdb.h>		 // getaddrinfo()
#include <stdio.h>		 // fprintf()
#include <stdlib.h>		 // malloc()
#include <string.h>		 // strcmp()
#include <sys/socket.h>	 // socket()
#include <sys/types.h>	 // socket()
#include <unistd.h>		 // dup()

#include <limits>  // std::numeric_limits<>

#define BOARD_MAX 3
#define SENSOR_MAX 5

#define S_MATCH_FIRST(X, Y, N) (!strncmp(X, Y, N))
#define S_EQUALS(X, Y) (!strcmp(X, Y))

#define CRLF "\r\n"

static void die(const char *msg) {
	if (errno) {
		perror(msg);
	} else {
		fprintf(stderr, "Function \"%s\" failed but errno is not set!\n", msg);
	}
	exit(EXIT_FAILURE);
}

static int connectTo(const char *__restrict hostname, const char *__restrict port) {
	struct addrinfo hints = {
		.ai_flags = AI_ADDRCONFIG,	 // Only locally available address types
		.ai_family = AF_UNSPEC,		 // Unspecified address familiy
		.ai_socktype = SOCK_STREAM,	 // TCP
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

static char *readLineAll(FILE *file) {	// Read a line of chars from a file
	size_t position = 0;
	size_t allocedSize = 8 * sizeof(char);	// Initial heap buffer size: 8 chars
	char *buf = (char *) malloc(allocedSize);
	if (!buf)
		die("malloc");

	while (1) {
		while (position < allocedSize) {  // Fill the buffer with data
			int c;
			if ((c = fgetc(file)) == EOF) {
				if (ferror(file))
					die("fgetc");
				if (position == 0) {  // EOF on a otherwise empty line
					free(buf);
					return nullptr;
				}
				buf[position] = '\0';
				return buf;
			}
			if (c == '\r') {  // Remove CR
				buf[position++] = '\0';
				continue;
			}
			if (c == '\n') {  // Remove LF
				buf[position] = '\0';
				return buf;
			}
			buf[position++] = (char) c;
		}
		position = allocedSize;
		allocedSize += 4 * sizeof(char);  // If not returned yet, allocate memory for another 4 chars
		buf = (char *) realloc(buf, allocedSize);
		if (!buf)
			die("realloc");
	}
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

char *requestFromBoard(uint8_t board, uint8_t sensor) {
	char address[15];
	snprintf(address, 15, "172.28.116.%i", board);
	FILE *rx = NULL;
	FILE *tx = NULL;
	int sock = connectTo(address, "80");  // Never fails
	initFileDescriptors(&rx, &tx, sock);

	fprintf(tx, "GET TEMP/SENSOR_VALUE/%i/C HTTP/1.1" CRLF, sensor);
	fflush(tx);

	char *msg = readLineAll(rx);
	if (!msg) {
		if (fclose(rx))	 // Close file descriptors
			die("fclose");
		if (fclose(tx))
			die("fclose");
		fprintf(stderr, "Server responded with EOF!\n");
		return nullptr;
	}

	if (!S_EQUALS(msg, "HTTP/1.1 200 OK")) {
		if (fclose(rx))	 // Close file descriptors
			die("fclose");
		if (fclose(tx))
			die("fclose");
		printf("ILLEGAL: %sA\n", msg);
		free(msg);
		return nullptr;
	}
	free(msg);

	for (size_t i = 0; i < 3; i++) {  // Remove header and empty line
		char *line = readLineAll(rx);
		if (line) {
			free(line);
			continue;
		}
		printf("Empty line!\n");
	}

	char *answer = readLineAll(rx);

	if (fclose(rx))	 // Close file descriptors
		die("fclose");
	if (fclose(tx))
		die("fclose");

	if (answer)
		return answer;

	die("Not an answer!\n");
	return nullptr;
}

float *getValuesFromBoard(uint8_t board) {
	const float nan = std::numeric_limits<float>::quiet_NaN();
	float *values = (float *) malloc(sizeof(float) * (SENSOR_MAX + 1));

	for (size_t i = 0; i < SENSOR_MAX; i++) {
		char *answer;
		do {
			answer = requestFromBoard(board, i);
		} while (!answer);	//TODO: endless loop possible

		if (S_MATCH_FIRST(answer, "INVALID READ", 12)) {
			values[i] = nan;
			free(answer);
			return values;
		}
		char *endPtr;
		float value = strtof(answer, &endPtr);

		if (answer == endPtr)
			printf("Not a float: \"%s\"\n", answer);  // Should not happen

		free(answer);
		values[i] = value;
	}
	values[SENSOR_MAX] = nan;
	return values;
}

int main(int argc, char **argv) {
	float *values;
	for (size_t board = 0; board < BOARD_MAX; board++) {
		values = getValuesFromBoard(board);
		printf("Answers from board %i:\n", board);
		size_t j = 0;
		while (!std::isnan(values[j])) {
			printf("Sensor %i: %.2f\n", j, values[j++]);
		}
		free(values);
	}
	return 0;
}
