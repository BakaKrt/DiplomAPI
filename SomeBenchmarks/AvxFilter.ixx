module;
#include <immintrin.h>

export module avxRule;

import sumFilterBase;
import std;

using std::string;


export class AvxRule : public SumFilterBase<AvxRule> {
public:
	AvxRule() { name = "avx"; }

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	inline void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& neighbours) const noexcept {
		constexpr size_t AVX2_WINDOW = 32;
		const size_t
			width = object.width(),
			height = object.height(),
			capacity = width * height;

		const size_t iter_count = capacity / AVX2_WINDOW;
		const size_t is_not_fit = capacity - AVX2_WINDOW * iter_count;

		T* dataPtr = object.data();
		T* resPtr = neighbours.data();

		const __m256i const_mask2 = _mm256_set1_epi8(2);
		const __m256i const_mask3 = _mm256_set1_epi8(3);

		auto sum = [&dataPtr, &resPtr, &const_mask2, &const_mask3](size_t load_offset) -> __m256i {
			__m256i original = _mm256_load_si256(reinterpret_cast<__m256i*>(dataPtr + load_offset));
			__m256i neighbours = _mm256_load_si256(reinterpret_cast<__m256i*>(resPtr + load_offset));

			__m256i mask3 = _mm256_cmpeq_epi8(neighbours, const_mask3);

			__m256i alive_mask = _mm256_or_si256(
				_mm256_cmpeq_epi8(neighbours, const_mask2),
				mask3
			);

			__m256i dead_mask = _mm256_andnot_si256(original, mask3);
			alive_mask = _mm256_and_si256(original, alive_mask);

			__m256i res = _mm256_or_si256(dead_mask, alive_mask);

			res = _mm256_min_epu8(res, _mm256_set1_epi8(1));

			return res;
		};

		for (size_t x = 0, i = 0; i < iter_count; x += AVX2_WINDOW, i++) {
			auto res = sum(x);
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(resPtr + x), res);
		}

		// если блок данных не умещается полностью
		if (is_not_fit) {
			size_t x = 0;
			if (capacity > AVX2_WINDOW) x = capacity - AVX2_WINDOW;

			auto res = sum(x);

			using std::memcpy;

			alignas(__m256i) uint8_t temp_to_remove[AVX2_WINDOW] {};
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(temp_to_remove), res);

			memcpy(resPtr + (capacity - is_not_fit), temp_to_remove + AVX2_WINDOW - is_not_fit, is_not_fit);
		}

	}
};