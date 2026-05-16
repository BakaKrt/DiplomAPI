module;

#include <emmintrin.h>
#include <memory>

export module sseRule;

import sumFilterBase;
import std;

using std::string;


export class SseRule : public SumFilterBase<SseRule> {
public:
	SseRule() { name = "sse"; }

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	inline void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& neighbours) const noexcept {
		constexpr size_t SSE_WINDOW = 16;
		const size_t
			width = object.width(),
			height = object.height(),
			capacity = width * height;

		const size_t iter_count = capacity / SSE_WINDOW;
		const size_t is_not_fit = capacity - SSE_WINDOW * iter_count;

		T* dataPtr = object.data();
		T* resPtr = neighbours.data();

		const __m128i const_mask2 = _mm_set1_epi8(2);
		const __m128i const_mask3 = _mm_set1_epi8(3);

		auto sum = [&dataPtr, &resPtr,&const_mask2, &const_mask3] (size_t load_offset) -> __m128i {
			__m128i original	= _mm_load_si128(reinterpret_cast<__m128i*>(dataPtr + load_offset));
			__m128i neighbours	= _mm_load_si128(reinterpret_cast<__m128i*>(resPtr + load_offset));

			__m128i mask3 = _mm_cmpeq_epi8(neighbours, const_mask3);

			__m128i alive_mask = _mm_or_si128(
				_mm_cmpeq_epi8(neighbours, const_mask2),
				mask3
			);

			__m128i dead_mask  = _mm_andnot_si128(original, mask3);
			alive_mask = _mm_and_si128(original, alive_mask);

			__m128i res = _mm_or_si128(dead_mask, alive_mask);

			res = _mm_min_epu8(res, _mm_set1_epi8(1));

			return res;
		};

		for (size_t x = 0, i = 0; i < iter_count; x += SSE_WINDOW, i++) {
			auto res = sum(x);
			_mm_store_si128(reinterpret_cast<__m128i*>(resPtr + x), res);
		}

		// если блок данных не умещается полностью
		if (is_not_fit) {
			size_t x = 0;
			if (capacity > SSE_WINDOW) x = capacity - SSE_WINDOW;

			auto res = sum(x);

			using std::memcpy;

			alignas(__m128i) uint8_t temp_to_remove[SSE_WINDOW] {};
			_mm_store_si128(reinterpret_cast<__m128i*>(temp_to_remove), res);

			memcpy(resPtr + (capacity - is_not_fit), temp_to_remove + SSE_WINDOW - is_not_fit, is_not_fit);
		}

	}
};