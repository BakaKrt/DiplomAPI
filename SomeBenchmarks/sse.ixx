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

	/// 0  1   2  9 10 11
	/// 3  4   5 12 13 14
	/// 6  7   8 15 16 17
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

#pragma region epi16
		//__m128i acc = _mm_set_epi16(
		//	object[0] +  object[1], object[2],
		//	object[3] +  object[4], object[5],
		//	object[6] +  object[7], object[8],
		//	object[9] + object[10], object[11]
		//);

		//__m128i temp = _mm_shufflelo_epi16(acc, _MM_SHUFFLE(2,3,0,1));
		//temp = _mm_shufflehi_epi16(temp, _MM_SHUFFLE(2,3,0,1));

		//acc = _mm_add_epi16(acc, temp);

		//res[0] = _mm_extract_epi16(acc, 0);
		//res[1] = _mm_extract_epi16(acc, 2);
		//res[2] = _mm_extract_epi16(acc, 4);
		//res[3] = _mm_extract_epi16(acc, 6);
#pragma endregion

#pragma region epi8_2
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
#pragma endregion

#pragma region epi8
		//__m128i acc = _mm_set_epi8(
		//	object[0], object[1], object[2], 0,
		//	object[3], object[4], object[5], 0,
		//	object[6], object[7], object[8], 0,
		//	object[9], object[10], object[11], 0
		//);

		//__m128i mask1 = _mm_set_epi8(
		//	14, 0, 0, 0,
		//	10, 0, 0, 0,
		//	6, 0, 0, 0,
		//	2, 0, 0, 0
		//);



		//__m128i temp = _mm_shuffle_epi8(acc, mask1);
		//__m128i add = _mm_add_epi8(acc, temp);

		//__m128i mask2 = _mm_set_epi8(
		//	13, 0, 0, 0,
		//	9, 0, 0, 0,
		//	5, 0, 0, 0,
		//	1, 0, 0, 0
		//);

		//acc = _mm_shuffle_epi8(acc, mask2);
		//add = _mm_add_epi8(add, acc);

		//uint32_t packed = _mm_cvtsi128_si32(add);

		//res[0] = (packed >> 0) & 0xFF;
		//res[1] = (packed >> 8) & 0xFF;
		//res[2] = (packed >> 16) & 0xFF;
		//res[3] = (packed >> 24) & 0xFF;

		//res[0] = _mm_extract_epi8(add, 15);
		//res[1] = _mm_extract_epi8(add, 11);
		//res[2] = _mm_extract_epi8(add, 7);
		//res[3] = _mm_extract_epi8(add, 3);
#pragma endregion

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
		
		const size_t size = 8;

		
		array<__m128i, size> top{};
		array<__m128i, size> low{};

		for (short i = 0; i < size; i++) {
			int smesh = 16 * i;
			top[i] = _mm_load_si128((__m128i*)(object.data() + smesh));

			low[i] = _mm_load_si128((__m128i*)(object.data() + 128 + smesh));
		}

		
		const __m128i mask1 = _mm_setr_epi8( 0, 1, 2, 3, 4, 5, 6, 7,  8,  9, 10, 11, 12, 13, 14,  1); // ok

		const __m128i mask2 = _mm_setr_epi8( 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15,  0); // ok

		const __m128i mask3 = _mm_setr_epi8( 1, 2, 3, 4, 5, 6, 7, 8,  9, 10, 11, 12, 13, 14, 15,  0); // ok
		
		const __m128i mask4 = _mm_setr_epi8(15, 0, 1, 2, 3, 4, 5, 6,  7,  8,  9, 10, 11, 12, 13, 14); // ok
		

		array<__m128i, size> temp{};

		for (short i = 0; i < size; i++) {
			top[i] = _mm_add_epi8(top[i], low[i]);		// r0 = r0 + r8
			temp[i] = _mm_shuffle_epi8(top[i], mask2);	// temp = mask2(r0)

			top[i] = _mm_shuffle_epi8(top[i], mask1);	// r0 = mask1(r0)
					

			low[i] = _mm_shuffle_epi8(low[i], mask3);	// r8 = mask3(r8) 
		}
		
		uint8_t __righ = _mm_extract_epi8(top[1], 0);
		temp[0] = _mm_insert_epi8(temp[0], __righ, 14);

		
		uint8_t __left = 0;

		int what_insert = 0;

		for (short i = 1; i < size - 1; i++) {
			__left = _mm_extract_epi8(temp[i - 1], 13);	// сосед слева
			__righ = _mm_extract_epi8(temp[i + 1], 15);	// сосед справа

			what_insert = ((int)(__left) << 8) | __righ;


			temp[i] = _mm_insert_epi16(temp[i], what_insert, 7); // temp = extract + insert
		}

		__left = _mm_extract_epi8(temp[size - 2], 13);
		temp[size - 1] = _mm_insert_epi8(temp[size - 1], __left, 15);

		for (short i = 0; i < size; i++) {
			top[i] = _mm_adds_epi8(top[i], temp[i]);

			top[i] = _mm_adds_epi8(top[i], low[i]);

			if (i != 0)
				top[i] = _mm_shuffle_epi8(top[i], mask4);
		}

		// левый
		_mm_storeu_si128((__m128i*)res.data(), top[0]);		// сохраняем результатов (последний невалидный)

		for (short i = 1; i < size; i++) {
			_mm_storeu_si128((__m128i*)(res.data() + (16 * (i - 1)) + 15), top[i]);
		}

		return res;
	}

	inline array<uint8_t, 70> run(array<uint8_t, 112>& object) const noexcept override {
		array<uint8_t, 70> res{};

		const size_t REGS_SIZE = 7;

		array<__m128i, REGS_SIZE> regs{};

		for (short i = 0; i < 7; i++) {
			regs[i] = _mm_load_si128((__m128i*) (object.data() + i * 16));
		}

		array<__m128i, 5> temp{};

		for (short i = 0; i < 5; i++) {
			temp[i] = regs[short(i + 1)];
		}

		for (short i = 1; i < REGS_SIZE - 2; i++) {
			regs[i] = _mm_adds_epi8(regs[i], regs[short(i + 1)]);	// r1 = r1 + r2 ...
		}
		regs[REGS_SIZE - 1] = regs[REGS_SIZE - 2];					// r6 = r5

		regs[0] = _mm_add_epi8(regs[0], regs[1]);					// r0 = r0 + r1
		regs[1] = _mm_add_epi8(regs[1], temp[0]);					// r1 = r1 + rt0
		regs[2] = _mm_add_epi8(regs[2], temp[3]);					// r2 = r2 + rt3
		regs[3] = _mm_add_epi8(regs[3], temp[4]);					// r3 = r3 + rt4
		regs[4] = _mm_add_epi8(regs[4], regs[REGS_SIZE-2]);			// r4 = r4 + r5

		const __m128i mask0 = _mm_setr_epi8(1, 2, 3, 4, 5, 6, 7, 8,  9, 10, 11, 12, 13, 14, 15, 0);
		const __m128i mask1 = _mm_setr_epi8(2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,  0, 1);

		array<__m128i, 4> sums{};

		for (short i = 0; i < 4; i++) {
			sums[i] = _mm_shuffle_epi8(regs[i], mask0);				// rsx = mask0(rx)

			const __m128i shuffled_center = _mm_shuffle_epi8(temp[i], mask0);
			sums[i] = _mm_sub_epi8(sums[i], shuffled_center);
		}

		for (short i = 0; i < 4; i++) { 
			sums[i] = _mm_add_epi8(sums[i], regs[0]);				// rsx = rsx + rx
			regs[i] = _mm_shuffle_epi8(regs[i], mask1);				// rx = mask1(rx)

			regs[i] = _mm_add_epi8(regs[i], sums[i]);				// rx = rx + rsx

			/*const __m128i shuffled_center = _mm_shuffle_epi8(temp[i], mask0);
			regs[i] = _mm_subs_epi8(regs[i], shuffled_center);*/
		}

		for (short i = 0; i < 4; i++) {
			_mm_storeu_si128((__m128i*)(res.data() + (14 * i)), regs[i]);
		}


		return res;
	}
};
