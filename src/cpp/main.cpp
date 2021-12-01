#include <math.h>	 // std::isnan()
#include <stdint.h>	 // uint8_t
#include <stdio.h>	 // fprintf()
#include <stdlib.h>	 // malloc()
#include <string.h>	 // strcmp()

#include "http-query.h"
#include "utility.h"
#include "utility.hpp"

#define BOARD_MAX 3
#define SENSOR_MAX 5

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
	static constexpr float nan = std::numeric_limits<float>::quiet_NaN();

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
	std::vector<float> v;
	float *values;
	for (size_t board = 0; board < BOARD_MAX; board++) {
		values = getValuesFromBoard(board);
		printf("Answers from board %i:\n", board);
		size_t j = 0;
		while (!std::isnan(values[j])) {
			v.push_back(values[j]);
			printf("Sensor %i: %.2f\n", j, values[j]);
			j++;
		}
		free(values);
	}
	printf("Average: %.2f\n", average(v));
	return 0;
}
