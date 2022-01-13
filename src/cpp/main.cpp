#include <unistd.h>	 // sleep()

#include <cerrno>	 // errno
#include <cstdlib>	 // std::free()
#include <cstring>	 // std::strerror()
#include <iostream>	 // std::cout
#include <numeric>	 // std::reduce()
#include <vector>	 // std::vector

#include "request.h"	   // requestApi()
#include "stringMacros.h"  // STRING_STARTS_WITH()

namespace {
	template <typename T>
	T vectorAverage(std::vector<T> const &vec) {
		if (vec.empty()) {
			return 0;
		}

		return std::reduce(vec.begin(), vec.end()) /
			   static_cast<T>(vec.size());
	}
}  // namespace

int main(int argc, char **argv) {
	requestInfo_t reqCelsius = {
		.host = const_cast<char *>("172.28.116.0"),
		.port = const_cast<char *>("80"),
		.request = const_cast<char *>("SENSOR/TEMPERATURE/0/VALUE/C"),
	};
	requestInfo_t reqBuzzer = {
		.host = const_cast<char *>("172.28.116.0"),
		.port = const_cast<char *>("80"),
		.request = const_cast<char *>("ACTOR/BUZZER/0/ON/1000"),
	};

	std::vector<float> floatStorage;

	while (1) {
		char *request;

		if (requestApi(&reqCelsius, &request) == 0) {
			std::cerr << "Temperature request failed! - " << std::strerror(errno) << std::endl;
			errno = 0;
			sleep(5);
			continue;
		}

		std::cout << "Received: \"" << request << "\" (C)" << std::endl;

		char *endPtr;
		float value = std::strtof(request, &endPtr);

		if (endPtr == request || *endPtr != '\0') {
			std::cerr << "Error parsing \"" << request << "\"" << std::endl;
			std::free(request);
			continue;
		}

		std::free(request);

		if (floatStorage.size() >= 4) {
			// Remove oldest element from the beginning of the vector
			floatStorage.erase(floatStorage.begin());
		}

		floatStorage.push_back(value);	// Add new element at the end of the vector

		float average = vectorAverage(floatStorage);
		std::cout << "Average(" << floatStorage.size() << "): " << average << " C" << std::endl;

		if (average < 20) {
			if (requestApi(&reqBuzzer, &request) == 0) {
				std::cerr << "Buzzing request failed! - " << std::strerror(errno) << std::endl;
				errno = 0;
				sleep(5);
				continue;
			}

			if (STRING_STARTS_WITH(request, "OK.")) {
				std::cout << "Buzzing successful!" << std::endl;
			} else {
				std::cerr << "Buzzing failed!" << std::endl;
			}

			std::free(request);
		}

		std::cout << "Sleeping for 15 seconds..." << std::endl;
		sleep(15);
	}

	return EXIT_SUCCESS;
}
