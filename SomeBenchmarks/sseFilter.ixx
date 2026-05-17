module;

#include <emmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <cstring>

export module sseRule;

import sumFilterBase;
import std;
import MasksCreator;

using std::string;
using std::array;

export class SseRule : public SumFilterBase<SseRule> {
private:
	array<uint8_t, 32> LUT;
public:
	
	SseRule() {
		name = "sse"; LUT = MasksCreator::Generate32bitLUTByBS({3}, {2, 3});
	}
	

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

		const __m128i lut_birth = _mm_load_si128((const __m128i*)LUT.data());
		const __m128i lut_survive = _mm_load_si128((const __m128i*)LUT.data() + 16);
		const __m128i one = _mm_set1_epi8(1);

		auto sum = [&dataPtr, &resPtr, &lut_birth, &lut_survive, &one] (size_t load_offset) -> __m128i {
			__m128i state = _mm_load_si128((const __m128i*)(dataPtr + load_offset));
			__m128i neigh = _mm_load_si128((const __m128i*)(resPtr + load_offset));

			// Результат для рождения и выживания (сразу по 16 клеток)
			__m128i birth_res = _mm_shuffle_epi8(lut_birth, neigh);
			__m128i surv_res = _mm_shuffle_epi8(lut_survive, neigh);

			// Маска живых клеток (0xFF там, где state == 1)
			__m128i live_mask = _mm_cmpeq_epi8(state, one);

			// Если клетка жива -> surv_res, иначе -> birth_res
			__m128i result = _mm_blendv_epi8(birth_res, surv_res, live_mask);
			return result;
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
			_mm_storeu_si128(reinterpret_cast<__m128i*>(temp_to_remove), res);

			memcpy(
				resPtr + (capacity - is_not_fit),
				temp_to_remove + SSE_WINDOW - is_not_fit,
				is_not_fit
			);
		}

	}
};