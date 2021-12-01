#pragma once
#ifndef HTTP_QUERY_H
#define HTTP_QUERY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>	// FILE

int connectTo(const char *__restrict hostname, const char *__restrict port);

void initFileDescriptors(FILE **rx, FILE **tx, int sock);

#ifdef __cplusplus
}
#endif

#endif	// HTTP_QUERY_H
