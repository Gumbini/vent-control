#pragma once
#ifndef REQUEST_H
#define REQUEST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char *host;
	char *port;
	char *request;
} requestInfo_t;

/**
 * @brief Request a response from an API
 * 
 * @param reqInfo a struct that contains all the information necessary to connect to an API
 * @param reqStorage the memory address of a @code{char *} in which the result string will be referenced
 * @return int @code{1} on success, @code{0} on failure
 */
int requestApi(requestInfo_t *reqInfo, char **reqStorage);

#ifdef __cplusplus
}
#endif

#endif	// REQUEST_H
