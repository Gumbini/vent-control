#pragma once
#ifndef SIPCALL_H
#define SIPCALL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char *domain;
	char *user;
	char *password;
	char *number;
} sip_data;

int callSip(const sip_data *config);

#ifdef __cplusplus
}
#endif

#endif	// SIPCALL_H
