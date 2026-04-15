#include <smmintrin.h> // SSE4.1

export module sse;

import std;
import test;

using std::string;
using std::array;


export class SSEv1Sum : public test<SSEv1Sum> {
public:
	SSEv1Sum() { name = "sse v1"; }

	inline const string getName_impl() const {
		return name;
	}

	/// <summary>
	/// Описание работы: 
	/// https://unidraw.io/app/board/0be060188688ad944421
	/// </summary>
	/// <param name="object"></param>
	/// <returns></returns>
	inline array<uint8_t, 126> run_impl(array<uint8_t, 256>& object) const noexcept  {
		array<uint8_t, 126> res{};
		
		const size_t mid_index = object.size() / 2;
		const size_t blocks_count = mid_index / 16;
		
		const size_t window_size = 2;

		array<__m128i, window_size> top {};
		array<__m128i, window_size> low {};
		array<__m128i, window_size> rr {};	// сохраняю сумму top + low

		array<__m128i, window_size> r2 {};
		array<__m128i, window_size> r3 {};


		const __m128i mask0 = _mm_setr_epi8(15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
		const __m128i mask1 = _mm_setr_epi8(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0);

		int offset = 0;
		uint8_t __left = 0, __right = 0;

		for (size_t block_id = 0; block_id < blocks_count - 1; block_id++) {

			if (block_id == 0) {
				for (short i = 0; i < window_size; i++) {
					top[i] = _mm_load_si128((__m128i*)(object.data() + offset));
					low[i] = _mm_load_si128((__m128i*)(object.data() + offset + mid_index));

					rr[i] = _mm_add_epi8(low[i], top[i]);

					offset += 16;
				}

				__left = _mm_extract_epi8(rr[0], 15);
			}
			else { [[likely]]
				top[0] = top[1];
				rr[0]  = rr[1];

				 
				top[1] = _mm_load_si128((__m128i*)(object.data() + offset));
				low[1] = _mm_load_si128((__m128i*)(object.data() + mid_index + offset));

				rr[1] = _mm_add_epi8(low[1], top[1]);

				offset += 16;
			}

			for (short i = 0; i < window_size; i++) {
				r2[i] = _mm_shuffle_epi8(rr[i], mask0);	// r2 = mask0(rr)
				r3[i] = _mm_shuffle_epi8(rr[i], mask1);	// r3 = mask1(rr)
			}


			if (block_id != 0) [[likely]] {
				r2[0] = _mm_insert_epi8(r2[0], __left, 0);
				__left = _mm_extract_epi8(rr[0], 15);
			}
			
			__right = _mm_extract_epi8(rr[1], 0);
			r3[0] = _mm_insert_epi8(r3[0], __right, 15);


			r2[0] = _mm_add_epi8(r2[0], r3[0]);		// r2 = r2 + r3

			low[0] = _mm_add_epi8(r2[0], rr[0]);	// r1 = r2 + rr

			low[0] = _mm_sub_epi8(low[0], top[0]);	// r1 = r1 - r0

			if (block_id == 0) { [[unlikely]]
				// если это первый блок, то перемешиваем (нулевой результат не нужен, там невалидные данные)
				low[0] = _mm_shuffle_epi8(low[0], mask1);
				_mm_store_si128((__m128i*)res.data(), low[0]);
			}
			else if (block_id == blocks_count - 2) { [[unlikely]]
				// если последний блок, то сохраняем предпоследний блок
				_mm_store_si128((__m128i*)(res.data() + 15 + (block_id - 1) * 16), low[0]);

				// вставляем в последний блок на нулевую позицию сумму соседей слева
				r2[1] = _mm_insert_epi8(r2[1], __left, 0);

				r2[1] = _mm_add_epi8(r2[1], r3[1]);
				low[1] = _mm_add_epi8(r2[1], rr[1]);
				low[1] = _mm_sub_epi8(low[1], top[1]);

				_mm_store_si128((__m128i*)(res.data() + 15 + block_id * 16), low[1]);
			}
			else { [[likely]]
				_mm_store_si128((__m128i*)(res.data() + 15 + (block_id - 1) * 16), low[0]);
			}
		}
		return res;
	}

	inline array<uint8_t, 70> run_impl(array<uint8_t, 112>& object) const noexcept  {
		array<uint8_t, 70> res{};

		__m128i r0, r1, r2, rres;

		int offset = 0;

		const size_t size = 7;

		array<uint8_t, size> indexes{};

		for (short i = 0; i < size; i++) {
			indexes[i] = i * 16;
		}


		for (short i = 0; i < size-2; i++) {
			r0 = _mm_load_si128((__m128i*)(object.data() + indexes[i    ]));
			r1 = _mm_load_si128((__m128i*)(object.data() + indexes[i + 1]));
			r2 = _mm_load_si128((__m128i*)(object.data() + indexes[i + 2]));

			rres = r1;					// rres = r1

			r1 = _mm_add_epi8(r1, r2);
			r0 = _mm_add_epi8(r0, r1);	// r0 = r2 + r1 + r0

			const __m128i mask0 = _mm_setr_epi8(15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
			const __m128i mask1 = _mm_setr_epi8(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0);

			r1 = _mm_shuffle_epi8(r0, mask0);	// r1 = mask0(r0)
			r2 = _mm_shuffle_epi8(r0, mask1);	// r2 = mask1(r0)


			r1 = _mm_add_epi8(r1, r2);
			r0 = _mm_add_epi8(r0, r1);			// r0 = r2 + r1 + r0

			r0 = _mm_sub_epi8(r0, rres);		// r0 = r0 - rres

			r0 = _mm_shuffle_epi8(r0, mask1);	// r0 = mask1(r0)

			_mm_storeu_si128((__m128i*)(res.data() + (i * 14)), r0);
		}

		return res;
	}


	template<typename T> requires allowed_type<T>
	inline Flat2DArray<T> run_horizontalSumImpl(Flat2DArray<T>& object) const noexcept {
		return object;
	}

	template<typename T> requires allowed_type<T>
	inline Flat2DArray<T> run_verticalSumImpl(Flat2DArray<T>& object) const noexcept {
		return object;
	}
};
