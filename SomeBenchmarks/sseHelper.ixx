#include <emmintrin.h>

export module sseHelper;

import std;

using std::array;


/// <summary>
/// Вся информация для SSE взята с сайта Intel: https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
/// Зеркало: https://www.laruence.com/sse/
/// </summary>
export namespace sseHelperNS {

#ifdef _DEBUG
	void print_float(const __m128& reg, std::string name) {
		alignas(__m128) float buf[4];
		_mm_store_ps(buf, reg);
		std::printf("%s:", name.c_str());
		for (int i = 0; i < 4; ++i) std::printf(" %f", buf[i]);
	};
	void print_uint8(const __m128i reg, std::string name) {
		alignas(__m128i)  uint8_t buf[16] {};
		_mm_store_si128((__m128i*)buf, reg);
		std::printf("%5s:", name.c_str());
		for (int i = 0; i < 16; ++i) std::printf("%3u ", (uint8_t) buf[i]);
	};

	void print_uint8_half(const __m128i reg, std::string name) {
		alignas(__m128i)  uint8_t buf[16] {};
		_mm_store_si128((__m128i*)buf, reg);
		std::printf("%5s:", name.c_str());
		for (int i = 0; i < 8; ++i) std::printf("%3u ", (uint8_t) buf[i]);
	};

	void print_two_uint(const array<__m128i, 2>& arr, const std::string& name) {
		for (size_t i = 0; i < arr.size(); i++) {
			// вывод в консоль в формате: name<i>: values
			print_uint8(arr[i], name + std::to_string(i));
		}
		std::printf("\n");
	}

#else
#define print_m128_float(reg, name) ((void)0)
#define print_m128_uint8(reg, name) ((void)0)
#define print_m128_uint8_half(reg, name) ((void)0)

#define print_two_m128i_uint(arr, name) ((void)0)
#endif // _DEBUG

	/// <summary>
	/// Задержки: 2
	/// Просто вертикальная сумма трёх регистров
	/// </summary>
	/// <param name="acc">Аккумулятор, здесь сохранится результат. Загрузить первую строку перед использованием</param>
	/// <param name="r1"></param>
	/// <param name="r2"></param>
	inline void justSum(__m128i &acc, __m128i r1, __m128i r2) {
		acc = _mm_add_epi8(acc, r1);
		acc = _mm_add_epi8(acc, r2);
	}

	/// <summary>
	/// Задержки: 4
	/// Сохраняет в acc сумму клетки по её соседям
	/// </summary>
	/// <param name="acc"></param>
	inline void sumNeighbours(__m128i& acc) {
		__m128i left, right;

		left  = _mm_srli_si128(acc, 1);
		right = _mm_slli_si128(acc, 1);

		acc = _mm_add_epi8(acc, left);
		acc = _mm_add_epi8(acc, right);
	}


#pragma region vertical_sums
	/// <summary>
	/// суммирует 3 строки вида:
	/// r0 =   0   1   2   3   4   5 ...
	/// r1 =  15  16  17  18  19  20 ...
	/// r2 =  30  31  32  33  34  35 ...
	/// r3 =  45  46  47  48  49  50 ...
	/// res[0] =  0 +  1 + 16 + 30 + 31,  0 +  1 +  2 + 15 + 17 + 30 + 31 + 32, ...
	/// res[1] = 15 + 16 + 31 + 45 + 46, 15 + 16 + 17 + 30 + 32 + 45 + 46 + 47, ...
	/// и т.д
	/// </summary>
	/// <param name="r0">Первая строка</param>
	/// <param name="r1">Вторая строка</param>
	/// <param name="r2">Третья строка</param>
	/// <param name="r3">Четвёртая строка</param>
	/// Общая задержка: 7 сложения (1) + 4 битовых сдвига (1) + 2 вычитания (1) = 13
	/// <returns></returns>
	inline array<__m128i, 2> verticalSum(__m128i r0, __m128i r1, __m128i r2, __m128i r3) noexcept {
		// [0] = средняя строка для первой тройки
		// [1] = средняя строка для последней тройки
		auto res = array<__m128i, 2> {r1, r2};

		r1 = _mm_add_epi8(r1, r2);	// r1 = средние строки

		r0 = _mm_add_epi8(r0, r1);	// r0 = первые 3 строки
		r3 = _mm_add_epi8(r3, r1);	// r3 = последние 3 строки

		r1 = _mm_slli_si128(r0, 1);
		r2 = _mm_srli_si128(r0, 1);

		r0 = _mm_add_epi8(r0, r1);
		r0 = _mm_add_epi8(r0, r2); // r0 = соседи слева + справа + он сам для верхней строки

		res[0] = _mm_sub_epi8(r0, res[0]); // res[0] = сумма соседей для верхней строки без центральной клетки

		r1 = _mm_slli_si128(r3, 1);
		r2 = _mm_srli_si128(r3, 1);

		r3 = _mm_add_epi8(r3, r1);
		r3 = _mm_add_epi8(r3, r2); // r3 = соседи слева + справа + он сам для нижней строки

		res[1] = _mm_sub_epi8(r3, res[1]); // res[1] = сумма соседей для нижней строки без центральной клетки
		return res;
	}

	/// <summary>
	/// суммирует 3 строки вида: <br/>
	/// r0 =   0   1   2   3   4   5 ... <br/>
	/// r1 =  15  16  17  18  19  20 ... <br/>
	/// r2 =  30  31  32  33  34  35 ...
	/// res[0] = 0 + 1 + 16+ 30 + 31
	/// res[1] = 0 + 1 + 2 + 15 + 17 + 30 + 31 + 32
	/// res[2] = 1 + 2 + 3 + 16 + 18 + 31 + 32 + 33 <br/>
	/// и т.д
	/// Общая задержка: 4 сложения (1) + 2 побитовых сдвига (1) + 1 вычитание (1) = 7 latency
	/// </summary>
	/// <param name="r0">Первая строка</param>
	/// <param name="r1">Вторая строка</param>
	/// <param name="r2">Третья строка</param>
	/// <returns></returns>
	inline __m128i verticalSum(__m128i r0, __m128i r1, __m128i r2) noexcept {
		__m128i mid_row = r1;

		// в r2 будет хранится вся сумма
		justSum(r2, r0, r1);
		

		r1 = _mm_slli_si128(r2, 1);		// сдвиг вправо на 1 байт (сосед справа)
		r0 = _mm_srli_si128(r2, 1);		// сдвиг влево на 1 байт (сосед слева)

		r2 = _mm_add_epi8(r2, r1);		// r1 = сам + сосед справа
		r2 = _mm_add_epi8(r2, r0);		// r1 = сам + сосед справа + сосед слева = 9 клеток

		r2 = _mm_sub_epi8(r2, mid_row);	// r1 = сумма всех 8 соседних клеток = 9 клеток - центральная клетка
		return r2;
	}

	/// <summary>
	/// суммирует 3 строки вида:
	/// r0 =   0   1   2   3   4   5 ...
	/// r1 =  15  16  17  18  19  20 ...
	/// r2 =  30  31  32  33  34  35 ...
	/// res[0] =						1 + 15 + 16,  0 + 2 + 15 + 16 + 17, ...
	/// res[1] = 0 + 1 + 2 + 15 + 17 + 30 + 31 + 32,  1 + 2 +  3 + 16 + 18 + 31 + 32 + 33
	/// и т.д
	/// Общая задержка: 6 сложений (1) + 4 побитовых сдвига (1) + 2 вычитание (1) = 12
	/// По сравнению с сложением сначала двух строк, а затем трёх, даст выйгрыш в 1 инструкцию
	/// </summary>
	/// <param name="r0">Первая строка</param>
	/// <param name="r1">Вторая строка</param>
	/// <param name="r2">Третья строка</param>
	/// <returns>res[0] - сумма для нулевой строки, res[1] - сумма для средней строки</returns>
	inline array<__m128i, 2> verticalSumFor2Rows(__m128i r0, __m128i r1, __m128i r2) noexcept {
		auto res = array<__m128i, 2> {r0, r1};

		// в temp1 сохраняю последнюю строку, а в temp2 сохраню сумму первых двух строк
		__m128i temp1 = r2, temp2;

		r0 = _mm_add_epi8(r0, r1);		// r0 = сумма первых двух строк
		temp2 = r0;						// сохраняю сумму для сложения с последней строки
		r1 = _mm_slli_si128(r0, 1);
		r2 = _mm_srli_si128(r0, 1);

		r0 = _mm_add_epi8(r0, r1);
		r0 = _mm_add_epi8(r0, r2);

		res[0] = _mm_sub_epi8(r0, res[0]);	// res[0] = сумма верхних двух строк, без центральных клеток

		r0 = temp2;
		r0 = _mm_add_epi8(r0, temp1);	// r0 = сумма всех строк

		r1 = _mm_slli_si128(r0, 1);
		r2 = _mm_srli_si128(r0, 1);

		r0 = _mm_add_epi8(r0, r1);
		r0 = _mm_add_epi8(r0, r2);

		res[1] = _mm_sub_epi8(r0, res[1]); // res[1] = сумма всех строк, без центральных клеток

		return res;
	}

	
	/// <summary>
	/// суммирует 2 строки вида:
	/// r0 =  0   1   2   3   4   5 ...
	///	r1 = 15  16  17  18  19  20 ...
	///	res[0] = 1 + 15 + 16
	///	res[1] = 0 + 2 + 15 + 16 + 17
	///	res[2] = 1 + 3 + 16 + 17 + 18 + 31 + 32 + 33
	///	и т.д.
	/// Общая задержка: 3 сложения (1) + 2 сдвига (1) + 1 вычитание (1) = 6
	/// </summary>
	/// <param name="r0">Регистр с загруженной первой строкой</param>
	/// <param name="r1">Регистр с загруженной второй строкой</param>
	/// <returns></returns>
	inline __m128i verticalSum(__m128i r0, __m128i r1) noexcept {
		__m128i temp_reg {};
		// в r0 хранятся середины (то есть потом из суммы соседей нужно будет вычесть r0)

		r1 = _mm_add_epi8(r0, r1);			// сумма первых двух строк
		__m128i r2 = _mm_srli_si128(r1, 1);	// r2 = r1 << 1 побитовый сдвиг влево
		temp_reg = _mm_slli_si128(r1, 1);	// temp_reg = r1 >> 1 побитовый сдвиг вправо

		r1 = _mm_add_epi8(r1, r2);
		r1 = _mm_add_epi8(r1, temp_reg);	// r1 = сумма всех соседей

		r1 = _mm_sub_epi8(r1, r0);			// r1 = r1 - r0
		return r1;
	};
#pragma endregion


	/*
	
	int offset = 0;
	uint8_t __left = 0, __right = 0;

	const size_t total_blocks_count = blocks_count + bool(remainder);

	for (size_t block_id = 0; block_id < total_blocks_count - 1; block_id++) {

		if (block_id == 0) {
			for (short i = 0; i < window_size; i++) {
				size_t obj_ptr_offset = offset;
				size_t obj_ptr_offset_mid = obj_ptr_offset + mid_index;

				top[i] = _mm_load_si128((__m128i*)(obj_ptr + offset));
				low[i] = _mm_loadu_si128((__m128i*)(obj_ptr + offset + mid_index));

				rr[i] = _mm_add_epi8(low[i], top[i]);

				offset += 16;
			}

			__left = _mm_extract_epi8(rr[0], 15);

			DEBUG_REGS("after init");
		}
		else {
			[[likely]]
			top[0] = top[1];
			rr[0] = rr[1];


			top[1] = _mm_load_si128((__m128i*)(obj_ptr + offset));
			low[1] = _mm_loadu_si128((__m128i*)(obj_ptr + mid_index + offset)); // unaligned, т.к. смещение может быть не кратно 16

			rr[1] = _mm_add_epi8(low[1], top[1]);

			offset += 16;

			DEBUG_REGS("after block_id != 0");
		}

			
		r2[0] = _mm_slli_si128(rr[0], 1);   // сдвиг вправо на 1 байт (влево по памяти)
		r3[0] = _mm_srli_si128(rr[0], 1);   // сдвиг влево на 1 байт (вправо по памяти)
			
		DEBUG_REGS("after shuffle");


		if (block_id != 0) [[likely]] {
			r2[0] = _mm_insert_epi8(r2[0], __left, 0);
			__left = _mm_extract_epi8(rr[0], 15);

			DEBUG_REGS("after insert r2 = insert(r2)");
		}

		__right = _mm_extract_epi8(rr[1], 0);
		r3[0] = _mm_insert_epi8(r3[0], __right, 15);


		r2[0] = _mm_add_epi8(r2[0], r3[0]);		// r2 = r2 + r3

		low[0] = _mm_add_epi8(r2[0], rr[0]);	// r1 = r2 + rr

		low[0] = _mm_sub_epi8(low[0], top[0]);	// r1 = r1 - r0

		DEBUG_REGS("after r1 = r2 + r3 - r0");

		if (block_id == 0) {
			[[unlikely]]
			// если это первый блок, то смещаем влево на 1 байт (нулевой результат не нужен, там невалидные данные)
			low[0] = _mm_srli_si128(low[0], 1);
			_mm_store_si128((__m128i*)res_ptr, low[0]);
			DEBUG_REGS("after STORE block_id = 0");
		}
		else if (block_id == total_blocks_count - 2) {
			[[unlikely]]
			// если последний блок, то сохраняем предпоследний блок
			_mm_store_si128((__m128i*)(res_ptr + 15 + (block_id - 1) * 16), low[0]);

			r2[1] = _mm_slli_si128(rr[1], 1);   // сдвиг вправо на 1 байт (влево по памяти)
			r3[1] = _mm_srli_si128(rr[1], 1);   // сдвиг влево на 1 байт (вправо по памяти)

			// вставляем в последний блок на нулевую позицию сумму соседей слева
			r2[1] = _mm_insert_epi8(r2[1], __left, 0);

			r2[1] = _mm_add_epi8(r2[1], r3[1]);
			low[1] = _mm_add_epi8(r2[1], rr[1]);
			low[1] = _mm_sub_epi8(low[1], top[1]);

			DEBUG_REGS("before STORE block_id - 2");

			// последний блок не полный
			if (bool(remainder)) {
				alignas(__m128i) char buf[16] {};
				_mm_store_si128((__m128i*)buf, low[1]);

				size_t offsetToEnd = mid_index - 1 - remainder;
				std::memcpy(res_ptr + offsetToEnd, buf, remainder - 1);
			}
			else {
				_mm_store_si128((__m128i*)(res_ptr + 15 + block_id * 16), low[1]);
			}
		}
		else {
			[[likely]]
			_mm_store_si128((__m128i*)(res_ptr + 15 + (block_id - 1) * 16), low[0]);
			DEBUG_REGS("after STORE block_id any");
		}
	}
	*/
}