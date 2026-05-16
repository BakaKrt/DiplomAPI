module;
#include <emmintrin.h>

export module sseHelper;

import std;
using std::array;


/// <summary>
/// Вся информация для SSE взята с сайта Intel: https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
/// Зеркало: https://www.laruence.com/sse/
/// </summary>
export namespace sseHelperNS {

#ifdef _DEBUG
	void print_float(const __m128& reg, std::string name) {
		alignas(__m128) float buf[4];
		_mm_store_ps(buf, reg);
		std::printf("%s:", name.c_str());
		for (int i = 0; i < 4; ++i) std::printf(" %f", buf[i]);
	};
	void print_uint8(const __m128i reg, std::string name) {
		alignas(__m128i)  uint8_t buf[16] {};
		_mm_store_si128((__m128i*)buf, reg);
		std::printf("%5s:", name.c_str());
		for (int i = 0; i < 16; ++i) std::printf("%3u ", (uint8_t) buf[i]);
	};

	void print_uint8_half(const __m128i reg, std::string name) {
		alignas(__m128i)  uint8_t buf[16] {};
		_mm_store_si128((__m128i*)buf, reg);
		std::printf("%5s:", name.c_str());
		for (int i = 0; i < 8; ++i) std::printf("%3u ", (uint8_t) buf[i]);
	};

	void print_two_uint(const array<__m128i, 2>& arr, const std::string& name) {
		for (size_t i = 0; i < arr.size(); i++) {
			// вывод в консоль в формате: name<i>: values
			print_uint8(arr[i], name + std::to_string(i));
		}
		std::printf("\n");
	}

#else
#define print_m128_float(reg, name) ((void)0)
#define print_m128_uint8(reg, name) ((void)0)
#define print_m128_uint8_half(reg, name) ((void)0)

#define print_two_m128i_uint(arr, name) ((void)0)
#endif // _DEBUG
}