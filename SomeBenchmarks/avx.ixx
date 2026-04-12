#include <immintrin.h>

export module avx;

import std;
import test;

using std::string;
using std::array;


export class AVXSum : public test {
public:
	AVXSum() { name = "avx"; }


	inline float run(array<float, 3>& object) const noexcept override {
		return 0.0f;
	}

	inline float run(array<float, 5>& object) const noexcept override {
		return 0.0f;
	}

	inline float run(array<float, 8>& object) const noexcept override {
		__m256 v = _mm256_load_ps(&object.data()[0]);

		// Шаг 1: Складываем нижние и верхние 128 бит
		// low = [1, 2, 3, 4], high = [5, 6, 7, 8]
		__m128 low = _mm256_castps256_ps128(v);         // Берем нижние 128 бит
		__m128 high = _mm256_extractf128_ps(v, 1);       // Извлекаем верхние 128 бит
		__m128 sum128 = _mm_add_ps(low, high);           // sum128 = [1+5, 2+6, 3+7, 4+8] = [6, 8, 10, 12]

		// Шаг 2: Горизонтальное сложение в 128-битном регистре
		// Используем _mm_hadd_ps дважды для суммирования 4-х значений в 1
		__m128 hadd1 = _mm_hadd_ps(sum128, sum128);      // [6+8, 10+12, 6+8, 10+12] = [14, 22, 14, 22]
		__m128 hadd2 = _mm_hadd_ps(hadd1, hadd1);        // [14+22, 14+22, ...] = [36, 36, ...]
		float result = _mm_cvtss_f32(hadd2);             // Извлекаем первый элемент: 36.0f
		return result;
	}

	inline array<float, 3> run(array<float, 15>& object) const noexcept override {
		return {};
	}

	inline array<float, 8> run(array<float, 24>& object) const noexcept override {
		array<float, 8> res{};
		return res;
	}

	inline array<float, 4> run(array<float, 12>& object) const noexcept override {
		array<float, 4> res{};
		return res;
	}

	inline array<uint8_t, 4> run(array<uint8_t, 12>& object) const noexcept override {
		array<uint8_t, 4> res{};
		return res;
	}

	inline array<uint8_t, 126> run(array<uint8_t, 256>& object) const noexcept override {
		array<uint8_t, 126> res{};

		return res;
	}
};
