#pragma once
#ifndef SIPCALL_H
#define SIPCALL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char *host;
	char *user;
	char *password;
	char *number;
} sipConfig_t;

int callSip(const sipConfig_t *config);

#ifdef __cplusplus
}
#endif

#endif	// SIPCALL_H
