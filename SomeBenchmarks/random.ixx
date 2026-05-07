export module random;

import std;
import Flat2DArray;

using std::shared_ptr, std::make_shared;
using std::vector, std::array;

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

export std::uint8_t randomUint8(uint8_t min = 0, uint8_t max = 255) {
	std::uniform_int_distribution<unsigned> dist(min, max);
	return static_cast<uint8_t>(dist(gen));
}

export 
vector<Flat2DArray<uint8_t>> generateAlignedMemoryForGameOfLife(size_t width, size_t height, size_t count, size_t alignment = 16, bool isNeighbours = true) {
	auto res = vector<Flat2DArray<uint8_t>>(); res.reserve(count);

	uint8_t min = 0, max = 8;
	if (isNeighbours == false) {
		max = 1;
	}

	const size_t capacity = width * height;

	for (size_t x = 0; x < count; x++) {
		auto arr = Flat2DArray<uint8_t>(width, height, alignment, false);
		uint8_t* ptr = arr.data();

		for (size_t i = 0; i < capacity; i++) {
			ptr[i] = randomUint8(min, max);
		}
		res.emplace_back(arr);
	}

	return res;
}


export template <typename T>
shared_ptr<Flat2DArray<T>> generateTestMemory(size_t width, size_t height, size_t align = 16) {
	auto obj = make_shared<Flat2DArray<T>>(width, height, align, false);
	auto ptr = obj->data();

	const size_t size = width * height;

	if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, bool>) {
		fillArrayRandomBool(size, (bool*) ptr, 50);
	}
	else if constexpr (std::is_same_v<T, float>) {
		fillArrayRandomFloat(size, (float*) ptr, 13);
	}
	else {
		fillArrayRandomInt(size, (int*) ptr, 13);
	}
	return obj;
}

export template <typename T>
vector<shared_ptr<Flat2DArray<T>>> generateVectorOfTestMemory(size_t len, size_t align, size_t width, size_t height) {
	vector<shared_ptr<Flat2DArray<T>>> vec {}; vec.reserve(len);

	for (size_t i = 0; i < len; i++) {
		vec.push_back(generateTestMemory<T>(width, height, align));
	}

	return vec;
}