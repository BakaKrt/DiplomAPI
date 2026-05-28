module;

#include <immintrin.h>


export module avxRule;

import sumFilterBase;
import std;
import MasksCreator;

using std::string;
using std::array;
using std::memcpy;


export class AvxRule : public SumFilterBase<AvxRule> {
private:
	array<uint8_t, 32> LUT_B;
	array<uint8_t, 32> LUT_S;
public:
	AvxRule() {
		name = "avx";
		LUT_B = MasksCreator::GenerateLUT_B({ 3 });
		LUT_S = MasksCreator::GenerateLUT_S({ 2, 3 });
	}

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	__declspec(noinline) void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& neighbours) const noexcept {
		constexpr size_t AVX2_WINDOW = 32;
		const size_t
			width = object.width(),
			height = object.height(),
			capacity = width * height;

		const size_t iter_count = capacity / AVX2_WINDOW;
		const size_t is_not_fit = capacity - AVX2_WINDOW * iter_count;

		T* dataPtr = object.data();
		T* resPtr = neighbours.data();

		const __m256i lutB = _mm256_load_si256((__m256i*) (LUT_B.data()));
		const __m256i lutS = _mm256_load_si256((__m256i*) (LUT_S.data()));


		auto sum = [&dataPtr, &resPtr, &lutB, &lutS] (size_t load_offset) -> __m256i {
			__m256i state = _mm256_load_si256(reinterpret_cast<__m256i*>(dataPtr + load_offset));
			__m256i neigh = _mm256_load_si256(reinterpret_cast<__m256i*>(resPtr + load_offset));

			const __m256i one = _mm256_set1_epi8(1);

			__m256i res_B = _mm256_shuffle_epi8(lutB, neigh);
			__m256i res_S = _mm256_shuffle_epi8(lutS, neigh);
			__m256i live_mask = _mm256_cmpeq_epi8(state, one);

			return _mm256_blendv_epi8(res_B, res_S, live_mask);
		};

		for (size_t x = 0, i = 0; i < iter_count; x += AVX2_WINDOW, i++) {
			auto res = sum(x);
			_mm256_store_si256(reinterpret_cast<__m256i*>(resPtr + x), res);
		}

		// если блок данных не умещается полностью
		if (is_not_fit) {
			size_t x = 0;
			if (capacity > AVX2_WINDOW) x = capacity - AVX2_WINDOW;

			auto res = sum(x);

			using std::memcpy;

			alignas(__m256i) uint8_t temp_to_remove[AVX2_WINDOW] {};
			_mm256_storeu_si256(reinterpret_cast<__m256i*>(temp_to_remove), res);

			memcpy(
				resPtr + (capacity - is_not_fit),
				temp_to_remove + AVX2_WINDOW - is_not_fit,
				is_not_fit
			);
		}

	}
};