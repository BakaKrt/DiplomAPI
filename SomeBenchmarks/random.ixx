export module random;

import std;

thread_local std::mt19937 gen(std::random_device {}());

export void fillArrayRandomBool(const size_t size, bool* array, int chance = 50) {
	std::uniform_int_distribution<> dist(1, 100);
	std::generate_n(array, size, [&] () { return dist(gen) <= chance; });
}

export void fillArrayRandomInt(const size_t size, int* array, int max_value = 100) {
	std::uniform_int_distribution<int> dist(0, max_value);
	for (size_t i = 0; i < size; ++i) {
		array[i] = dist(gen);
	}
}

export void fillArrayRandomFloat(const size_t size, float* array, int max_value = 100) {
	std::uniform_real_distribution<float> dist(0.0f, float(max_value));

	for (size_t i = 0; i < size; ++i) {
		array[i] = dist(gen);
	}
}