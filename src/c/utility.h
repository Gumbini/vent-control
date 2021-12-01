#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#define CRLF "\r\n"

#define S_MATCH_FIRST(X, Y, N) (!strncmp(X, Y, N))
#define S_EQUALS(X, Y) (!strcmp(X, Y))

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

void die(const char *msg);

char *readLineAll(FILE *file);

#ifdef __cplusplus
}
#endif

#endif	// UTILITY_H
