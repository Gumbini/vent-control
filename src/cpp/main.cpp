#include <cerrno>	 // errno
#include <cmath>	 // std::isnan()
#include <cstdint>	 // uint8_t
#include <cstdlib>	 // free()
#include <cstring>	 // strerror()
#include <iostream>	 // count

#include "request.h"
#include "utility.h"
#include "utility.hpp"

int main(int argc, char **argv) {
	requestInfo_t reqInfo = {
		.host = const_cast<char *>("172.28.116.0"),
		.port = const_cast<char *>("80"),
		.request = const_cast<char *>("SENSOR/TEMPERATURE/0/VALUE/C"),
	};

	char *request;

	if (requestApi(&reqInfo, &request) == 0) {
		std::cerr << "Connection fail! - " << strerror(errno) << std::endl;
	}

	std::cout << request << std::endl;
	free(request);

	return EXIT_SUCCESS;
}
