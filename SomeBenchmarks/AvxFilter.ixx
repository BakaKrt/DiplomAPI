module;
#include <immintrin.h>
#include <cstring>

export module avxRule;

import sumFilterBase;
import std;
import MasksCreator;

using std::string;
using std::array;


export class AvxRule : public SumFilterBase<AvxRule> {
private:
	array<uint8_t, 32> LUT;
public:
	AvxRule() {
		name = "avx"; LUT = MasksCreator::Generate32bitLUTByBS({ 3 }, { 2, 3 });
	}

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

		const __m256i lut = _mm256_load_si256((__m256i*) (LUT.data()));
		const __m256i one = _mm256_set1_epi8(1);
		const __m256i offset16 = _mm256_set1_epi8(16);


		auto sum = [&dataPtr, &resPtr, &lut, &one, &offset16](size_t load_offset) -> __m256i {
			__m256i state = _mm256_load_si256(reinterpret_cast<__m256i*>(dataPtr + load_offset));
			__m256i neigh = _mm256_load_si256(reinterpret_cast<__m256i*>(resPtr + load_offset));

			// Формируем индекс: state==1 ? neigh+16 : neigh
			__m256i live_mask = _mm256_cmpeq_epi8(state, one);   // 0xFF где жива
			__m256i state_off = _mm256_and_si256(live_mask, offset16);
			__m256i idx = _mm256_or_si256(neigh, state_off);     // neigh + 0 или 16

			// Одна перестановка даёт всё правило
			__m256i result = _mm256_shuffle_epi8(lut, idx);

			return result;
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