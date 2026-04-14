#include <smmintrin.h> // SSE4.1

export module sse;

import std;
import test;

using std::string;
using std::array;


export class SSEv1Sum : public test {
public:
	SSEv1Sum() { name = "sse v1"; }

	inline float run(array<float, 3>& object) const noexcept override {
		__m128 acc = _mm_set_ps(0.0f, object[2], object[1], object[0]);		// [0,  a1, a2, a3]
		__m128 temp = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1));	// [a2, a3,  0, a1]
		acc = _mm_add_ps(acc, temp);										// acc = [0+a2, a1+a3, a2+0, a3+a1]
		temp = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(1, 1, 1, 1));			// temp = [acc[1], acc[1], acc[1], acc[1]] = [a1+a3, a1+a3, a1+a3, a1+a3]
		acc = _mm_add_ss(acc, temp);										// acc = [(a0+a2)+(a1+a3), a1+a3, a2+a0, a3+a1]
		return _mm_cvtss_f32(acc);
	}

	inline float run(array<float, 5>& object) const noexcept override {
		object[3] += object[4];                                         // [a0,    a1, a2, a3+a4, a4]

		__m128 acc = _mm_load_ps(object.data());                        // [a0,    a1, a2, a3+a4]
		__m128 temp = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1));// [a2, a3+a4, a0,    a1]

		acc = _mm_add_ps(acc, temp);                                    // [a0+a2, a1+a3+a4, a2+a0, a3+a4+a1]

		temp = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(3, 3, 3, 3));       // [      a1+a3+a4, a1+a3+a4, a1+a3+a4, a1+a3+a4]
		acc = _mm_add_ss(acc, temp);                                    // [a0+a1+a2+a3+a4, a1+a3+a4,    a2+a0, a3+a4+a1]

		return _mm_cvtss_f32(acc);                                      // a0+a1+a2+a3+a4
	}

	inline float run(array<float, 8>& object) const noexcept override {
		__m128 acc = _mm_load_ps(&object.data()[0]);			// [   a0,    a1,    a2,    a3]
		__m128 temp = _mm_load_ps(&object.data()[4]);			// [   a4,    a5,    a6,    a7]
		acc = _mm_add_ps(acc, temp);							// [a0+a4, a1+a5, a2+a6, a3+a7]

		// Горизонтальное сложение в __m128
		temp = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1)); // [a2+a6, a3+a7,a0+a4,a1+a5]
		acc = _mm_add_ps(acc, temp);							// [a0+a4+a2+a6, a1+a5+a3+a7, ...]

		temp = _mm_movehl_ps(temp, acc);						// [a1+a5+a3+a7, ??, ??, ??]
		acc = _mm_add_ss(acc, temp);							// [total, ??, ??, ??]
		float result = _mm_cvtss_f32(acc);						// total
		return result;
	}

	inline float calc(__m128 acc, __m128 temp) const noexcept {
		acc = _mm_add_ps(acc, temp);

		temp = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1));
		acc = _mm_add_ps(acc, temp);

		temp = _mm_movehl_ps(temp, acc);
		acc = _mm_add_ss(acc, temp);
		return _mm_cvtss_f32(acc);
	}

	///  0 1 2  9 10
	///  3 4 5 11 12
	///  6 7 8 13 14
	inline array<float, 3> run(array<float, 15>& object) const noexcept override {
		array<float, 3> res{};

		__m128 acc1 = _mm_load_ps(&object.data()[0]);
		__m128 temp1 = _mm_load_ps(&object.data()[5]);

		__m128 acc2 = _mm_set_ps(object[1], object[2], object[4], object[7]);
		__m128 temp2 = _mm_set_ps(object[8], object[9], object[11], object[13]);

		__m128 acc3 = _mm_set_ps(object[2], object[5], object[8], object[9]);
		__m128 temp3 = _mm_set_ps(object[10], object[12], object[13], object[14]);

		res[0] = calc(acc1, temp1);
		res[1] = calc(acc2, temp2);
		res[2] = calc(acc3, temp3);

		return res;
	}

	inline array<float, 4> run(array<float, 12>& object) const noexcept override {
		array<float, 4> res{};

		__m128 acc1 = _mm_set_ps(object[0] + object[1], object[2], object[3] + object[4], object[5]);	// [0+1,   2, 3+4,   5]
		__m128 temp1 = _mm_shuffle_ps(acc1, acc1, _MM_SHUFFLE(2, 3, 0, 1));							// [  2, 0+1,   5, 3+4]
		acc1 = _mm_add_ps(acc1, temp1);																// [res0, res0, res1, res1]


		__m128 acc2 = _mm_set_ps(object[6] + object[7], object[8], object[9] + object[10], object[11]);	// [6+7,   8, 9+10,   11]
		__m128 temp2 = _mm_shuffle_ps(acc2, acc2, _MM_SHUFFLE(2, 3, 0, 1));	// [  8, 6+7,   11, 9+10]
		acc2 = _mm_add_ps(acc2, temp2);										// [res2, res2, res3, res3]

		res[1] = _mm_cvtss_f32(acc1);
		res[3] = _mm_cvtss_f32(acc2);

		acc1 = _mm_shuffle_ps(acc1, acc1, _MM_SHUFFLE(2, 2, 2, 2));			// [res0, res0, res0, res0]
		acc2 = _mm_shuffle_ps(acc2, acc2, _MM_SHUFFLE(2, 2, 2, 2));			// [res2, res2, res2, res2]

		res[0] = _mm_cvtss_f32(acc1);
		res[2] = _mm_cvtss_f32(acc2);

		return res;
	}

	/// 0   1  2  9 10 11
	/// 3   4  5 12 13 14
	/// 6   7  8 15 16 17
	/// 18 19 20 21 22 23
	inline array<float, 8> run(array<float, 24>& object) const noexcept override {
		array<float, 8> res{};

		array<__m128, 16> registers{};

		registers[0] = _mm_load_ps(&object.data()[0]);
		registers[8] = _mm_load_ps(&object.data()[3]);								// 7  f
		registers[1] = _mm_load_ps(&object.data()[5]);								// 4

		//__m128 temp1 = _mm_set_ps(0.0f, object[2], object[5], object[8]);

		registers[2] = _mm_set_ps(object[1], object[2], object[4], object[7]);
		registers[3] = _mm_set_ps(object[8], object[9], object[12], object[15]);	// 5
		registers[12] = _mm_set_ps(object[5], object[8], object[12], object[13]);	// 15 f


		registers[4] = _mm_set_ps(object[2], object[5], object[8], object[9]);
		registers[6] = _mm_load_ps(&object.data()[9]);								// 13 f
		registers[5] = _mm_set_ps(object[10], object[13], object[15], object[16]);	// 12

		registers[7] = _mm_load_ps(&object.data()[14]);								// 13 s

		registers[9] = _mm_set_ps(object[8], object[18], object[19], object[20]);	// 7  s

		registers[10] = _mm_set_ps(object[4], object[5], object[7], object[12]);
		registers[11] = _mm_set_ps(object[15], object[19], object[20], object[21]);	// 8

		registers[13] = _mm_set_ps(object[16], object[20], object[21], object[22]);	// 15 s

		registers[14] = _mm_load_ps(&object.data()[12]);
		registers[15] = _mm_set_ps(object[17], object[21], object[22], object[23]);	// 16

		res[0] = calc(registers[0], registers[1]);
		res[1] = calc(registers[2], registers[3]);
		res[2] = calc(registers[4], registers[5]);
		res[3] = calc(registers[6], registers[7]);
		res[4] = calc(registers[8], registers[9]);
		res[5] = calc(registers[10], registers[11]);
		res[6] = calc(registers[12], registers[13]);
		res[7] = calc(registers[14], registers[15]);

		return res;
	}

	inline array<uint8_t, 4> run(array<uint8_t, 12>& object) const noexcept override {
		array<uint8_t, 4> res{};

		__m128i acc = _mm_set_epi8(
			object[0], object[1], object[2], object[3],
			object[4], object[5], object[6], object[7],
			object[8], object[9], object[10], object[11],
			0, 0, 0, 0
		);

		__m128i mask = _mm_set_epi8(
			0, 13, 0, 0,
			10, 0, 0, 8,
			0, 0, 4, 0,
			15, 12, 9, 6
		);

		__m128i temp = _mm_shuffle_epi8(acc, mask);
		__m128i sum = _mm_adds_epi8(acc, temp);

		__m128i mask2 = _mm_set_epi8(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			14, 11, 8, 5
		);

		__m128i sum2 = _mm_shuffle_epi8(sum, mask2);

		__m128i sum3 = _mm_adds_epi16(sum, sum2);

		auto packed = _mm_cvtsi128_si32(sum3);

		res[0] = (packed >> 0) & 0xFF;
		res[1] = (packed >> 8) & 0xFF;
		res[2] = (packed >> 16) & 0xFF;
		res[3] = (packed >> 24) & 0xFF;

		return res;
	}

	/// <summary>
	/// Описание работы: 
	/// https://unidraw.io/app/board/0be060188688ad944421
	/// </summary>
	/// <param name="object"></param>
	/// <returns></returns>
	inline array<uint8_t, 126> run(array<uint8_t, 256>& object) const noexcept override {
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

	inline array<uint8_t, 70> run(array<uint8_t, 112>& object) const noexcept override {
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
};
