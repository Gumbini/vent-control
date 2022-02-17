#include <unistd.h> // sleep()

#include <cerrno>   // errno
#include <cstdlib>  // std::free()
#include <cstring>  // std::strerror()
#include <iostream> // std::cout
#include <numeric>  // std::reduce()
#include <vector>   // std::vector

#include "request.h"      // requestApi()
#include "stringMacros.h" // STRING_STARTS_WITH()

#define MAX_DEVIATION_FROM_AVERAGE_KELVIN (4.0F)
#define MIN_TEMPERATURE_CELSIUS           20

#define SLEEP_AFTER_FAILURE_SEC 5
#define SLEEP_AFTER_SUCCESS_SEC 15

#define AVERAGE_MAX_VALUES ((60 / SLEEP_AFTER_SUCCESS_SEC) * 60 * 24)
// 4 readings/second * 60 seconds/minute * 24 minutes/day

namespace {
	template <typename T>
	T vectorAverage(std::vector<T> const &vec) {
		if (vec.empty()) {
			return 0;
		}

		return std::reduce(vec.begin(), vec.end()) / static_cast<T>(vec.size());
	}

	bool buzz(requestInfo_t *reqInfo) {
		char *request;
		if (requestApi(reqInfo, &request) == 0) {
			std::cerr << "Buzzing request failed! - " << std::strerror(errno) << std::endl;
			errno = 0;
			return false;
		}

		if (STRING_STARTS_WITH(request, "OK.")) {
			std::cout << "Buzzing successful!" << std::endl;
			std::free(request);
			return true;
		}

		std::cerr << "Buzzing failed!" << std::endl;
		std::free(request);
		return false;
	}
} // namespace

int main(int argc, char **argv) {
	requestInfo_t reqCelsius = {
		.host    = const_cast<char *>("172.28.116.0"),
		.port    = const_cast<char *>("80"),
		.request = const_cast<char *>("SENSOR/TEMPERATURE/0/VALUE/C"),
	};
	requestInfo_t reqBuzzer = {
		.host    = const_cast<char *>("172.28.116.0"),
		.port    = const_cast<char *>("80"),
		.request = const_cast<char *>("ACTOR/BUZZER/0/ON/1000"),
	};

	std::vector<float> floatStorage;

	while (1) {
		char *request;

		if (requestApi(&reqCelsius, &request) == 0) {
			std::cerr << "Temperature request failed! - " << std::strerror(errno) << std::endl;
			errno = 0;
			sleep(SLEEP_AFTER_FAILURE_SEC);
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

		if (floatStorage.size() >= AVERAGE_MAX_VALUES) {
			// Remove oldest element from the beginning of the vector
			floatStorage.erase(floatStorage.begin());
		}

		floatStorage.push_back(value); // Add new element at the end of the vector

		// Buzz if the value is below MIN_TEMPERATURE_CELSIUS
		if (value < MIN_TEMPERATURE_CELSIUS) {
			std::cout << "Actual value dropped below " << MIN_TEMPERATURE_CELSIUS << " C!" << std::endl;

			if (!buzz(&reqBuzzer)) {
				sleep(SLEEP_AFTER_FAILURE_SEC);
				continue;
			}

			std::cout << "Sleeping for " << SLEEP_AFTER_SUCCESS_SEC << " seconds..." << std::endl;
			sleep(SLEEP_AFTER_SUCCESS_SEC);
			continue;
		}

		float average = vectorAverage(floatStorage);
		std::cout << "Average(values: " << floatStorage.size() << "): " << average << " C" << std::endl;

		// Buzz if the value deviates more than MAX_DEVIATION_FROM_AVERAGE_KELVIN from the average
		// No std::abs() because it does not matter if it gets warmer at some point
		if ((average - value) > MAX_DEVIATION_FROM_AVERAGE_KELVIN) {
			std::cout << "Actual value deviated more than " << MAX_DEVIATION_FROM_AVERAGE_KELVIN
					  << " K from the average!" << std::endl;

			if (!buzz(&reqBuzzer)) {
				sleep(SLEEP_AFTER_FAILURE_SEC);
				continue;
			}
		}

		std::cout << "Sleeping for " << SLEEP_AFTER_SUCCESS_SEC << " seconds..." << std::endl;
		sleep(SLEEP_AFTER_SUCCESS_SEC);
	}

	return EXIT_SUCCESS;
}
