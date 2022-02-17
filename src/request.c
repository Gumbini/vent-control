#include "request.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

struct dataChunk {
	char *data;
	size_t size;
};

static size_t processData(char *itemBuf, size_t itemSize, size_t nItems, void *userPtr) {
	size_t itemBufSize      = itemSize * nItems;
	struct dataChunk *chunk = userPtr;

	char *newBuf = realloc(chunk->data, chunk->size + itemBufSize + 1);
	if (newBuf == NULL) {
		return 0; // ENOMEM
	}

	memcpy(newBuf + chunk->size, itemBuf, itemBufSize);
	newBuf[chunk->size + itemBufSize] = '\0';

	chunk->data = newBuf;
	chunk->size += itemBufSize;

	return itemBufSize;
}

char *requestUrl(const char *url) {
	CURL *curl = curl_easy_init();

	if (curl == NULL) {
		return NULL;
	}

	struct dataChunk chunk = {
		.data = NULL,
		.size = 0,
	};

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &processData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);

	CURLcode cc = curl_easy_perform(curl);

	if (cc != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform: %s\n", curl_easy_strerror(cc));
		free(chunk.data);
		return NULL;
	}

	curl_easy_cleanup(curl);

	return chunk.data;
}
