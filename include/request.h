#pragma once
#ifndef REQUEST_H
#define REQUEST_H

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief Request something from the network
	 *
	 * @param url an URL that contains all the necessary information
	 * @return a @code{char *} which can be passed to @code{free(3)} on success, @code{NULL} on failure
	 */
	char *requestUrl(const char *url);

#ifdef __cplusplus
}
#endif

#endif // REQUEST_H
