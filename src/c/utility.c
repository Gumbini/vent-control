#include "utility.h"

#include <errno.h>
#include <stdlib.h>

void die(const char *msg) {
	if (errno) {
		perror(msg);
	} else {
		fprintf(stderr, "[DIE] NO ERRNO: \"%s\"\n", msg);
	}
	exit(EXIT_FAILURE);
}

char *readLineAll(FILE *file) {	 // Read a line of chars from a file
	size_t position = 0;
	size_t allocedSize = 8 * sizeof(char);	// Initial heap buffer size: 8 chars
	char *buf = malloc(allocedSize);
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
					return NULL;
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
		buf = realloc(buf, allocedSize);
		if (!buf)
			die("realloc");
	}
}
