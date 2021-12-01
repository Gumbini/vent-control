#include "utility.hpp"

#include <numeric>	// std::reduce()

float average(std::vector<float> const &v) {
	if (v.empty())
		return 0;

	const float count = static_cast<float>(v.size());
	return std::reduce(v.begin(), v.end()) / count;
}
