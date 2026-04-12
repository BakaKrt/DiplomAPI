#include <cstdint>
#include <emmintrin.h>

export module Useful;

import std;

export namespace Useful {

	using byte = std::uint8_t;
	using std::array;

	std::random_device rd;

#pragma region Random arrays
	float* RandomFloatArray(const size_t len, const float l, const float r)
	{
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(l, r);

		float* ptr = new float[len];
		for (size_t pos = 0; pos < len; pos++) {
			ptr[pos] = dist(gen);
		}

		return ptr;
	}

	float* RandomFloatCanonical(const size_t len)
	{
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(0.0, 1.0);

		float* ptr = new float[len];
		for (size_t pos = 0; pos < len; pos++) {
			float rand = dist(gen);
			ptr[pos] = rand;
		}

		return ptr;
	}

	byte* RandomByteArray(const size_t size, const byte l, const byte r)
	{
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(l, r);

		byte* ptr = new byte[size];
		for (size_t pos = 0; pos < size; pos++) {
			ptr[pos] = (byte)dist(gen);
		}

		return ptr;
	}

	bool* RandomBoolArray(const size_t size, int chance = 50)
	{
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(1, 100);

		bool* ptr = new bool[size];
		for (size_t pos = 0; pos < size; pos++) {
			if (dist(gen) >= chance) ptr[pos] = true;
			else ptr[pos] = false;
		}

		return ptr;
	}

	template <typename T>
	void FillArrayRandomBool(const size_t size, T* array, int chance = 50) {
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(1, 100);

		for (size_t pos = 0; pos < size; pos++) {
			if (dist(gen) >= chance) array[pos] = true;
			else array[pos] = false;
		}

		return;
	}

	void FillVectorRandomBool(std::vector<bool>& vec, int chance = 50) {
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(1, 100);

		for (size_t i = 0; i < vec.size(); ++i) {
			vec[i] = dist(gen) <= chance;
		}
	}
#pragma endregion

#pragma region Threads Count
	int GetThreadsCount(int count) {
		static auto ThreadsCount = std::thread::hardware_concurrency();
		if (count == 0) return (int)ThreadsCount;
		return count;
	}
#pragma endregion

#pragma region SSE sums
	float sumSSE(array<float, 8> object) {

		__m128 v1 = _mm_load_ps(&object.data()[0]);
		__m128 v2 = _mm_load_ps(&object.data()[4]);
		__m128 partial_sum = _mm_add_ps(v1, v2);

		__m128 shuf = _mm_shuffle_ps(partial_sum, partial_sum, _MM_SHUFFLE(2, 3, 0, 1));
		__m128 sums = _mm_add_ps(partial_sum, shuf);
		shuf = _mm_movehl_ps(shuf, sums);
		sums = _mm_add_ss(sums, shuf);
		float result = _mm_cvtss_f32(sums);
		return result;
	}

	int32_t sumSSE(const array<int32_t, 8>& object) {
		__m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&object[0]));
		__m128i v2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&object[4]));

		__m128i partial_sum = _mm_add_epi32(v1, v2);

		__m128i shuf1 = _mm_shuffle_epi32(partial_sum, _MM_SHUFFLE(3, 2, 1, 0));
		__m128i sum1 = _mm_add_epi32(partial_sum, shuf1);

		__m128i shuf_a = _mm_shuffle_epi32(partial_sum, _MM_SHUFFLE(2, 3, 0, 1));
		__m128i sum_ab = _mm_add_epi32(partial_sum, shuf_a);

		__m128i shuf_final = _mm_shuffle_epi32(sum_ab, _MM_SHUFFLE(1, 0, 3, 2));
		__m128i final_pair = _mm_add_epi32(sum_ab, shuf_final);
		int32_t result = _mm_cvtsi128_si32(final_pair);
		return result;
	}
#pragma endregion

}