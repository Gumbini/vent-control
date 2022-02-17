#pragma once
#ifndef UTILITY_H
#define UTILITY_H

#define CRLF "\r\n"

#include "stringMacros.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

	void die(const char *msg);

	char *readLineAll(FILE *file);

#ifdef __cplusplus
}
#endif

#endif // UTILITY_H
