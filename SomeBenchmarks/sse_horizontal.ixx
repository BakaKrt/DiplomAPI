#include <smmintrin.h>

export module sse_horizontal;

import std;
import test;

import sseHelper;

using std::string;
using std::array;

using std::memcpy;
using std::printf;

using namespace sseHelperNS;

export class SSEv2Sum : public test<SSEv2Sum> {
public:
	SSEv2Sum() { name = "sse v2"; }

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	inline void test_runImpl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		run_horizontalSumAll(object, to_save);
	}



	inline static __m128i justSum(__m128i r0, __m128i r1, __m128i r2) noexcept {
		// никак не получается выйти в паралеллизм на уровне инструкций, т.к. результат одного требуется для результата следующей операции
		__m128i result;
		result = _mm_add_epi8(r0, r1);
		result = _mm_add_epi8(result, r2);

		return result;
	}
	/// <summary>
	/// Результат суммы соседей слева и справа сохраняется в регистр r0
	/// </summary>
	/// <param name="r0"></param>
	/// <param name="r1"></param>
	/// <param name="r2"></param>
	inline static void justSum(__m128i* r0, __m128i r1, __m128i r2) noexcept {
		// никак не получается выйти в паралеллизм на уровне инструкций, т.к. результат одного требуется для результата следующей операции
		*r0 = _mm_add_epi8(*r0, r1);
		*r0 = _mm_add_epi8(*r0, r2);
	}

	inline static __m128i getNeighboursByVertical(__m128i r0) noexcept {
		// параллелизм на уровне инструкций (операции не зависят друг от друга и процессор может выполнить их одновременно)
		return
			_mm_add_epi8(r0,
				_mm_add_epi8(
					_mm_srli_si128(r0, 1), _mm_slli_si128(r0, 1)
				)
		);
	}


	/// <summary>
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="object"></param>
	/// <param name="to_save"></param>
	template<typename T> requires allowed_type<T>
	inline void run_horizontalSumAll(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		const size_t width  = object.width();
		const size_t height = object.height();

		auto res_ptr = to_save.data();
		auto obj_ptr = object.data();

		// индекс начала нижней строки (то есть 1-ой строки) [0-ая это верхняя]
		const size_t mid_index = width;

		// количество блоков, которые полностью помещаются в ширину
		const size_t blocks_count = mid_index / 16;

		// количество блоков, которые не полностью помещаются в ширину. Если ширина не кратна 16, то последний блок будет обрабатываться отдельно
		const size_t remainder = width % 16;

		constexpr size_t window_size = 2;

		array<__m128i, window_size> top {};
		array<__m128i, window_size> mid {};
		array<__m128i, window_size> low {};

		array<__m128i, window_size> r3sum {}; // [0] для суммы 2 строк, [1] для суммы 3 строк

		/*
		граница двух регистров:
		r00: ... 14 15 | r01: 16 17 ...
		r10: ... 29 30 | r11: 31 32 ...

		так как перекрытия нет, нужно сначала вертикально сложить r0sum = (r00, r10) и r1sum = (r01, r11).
		Взять крайние элементы, в данном случае left_sum = (15+30) и right_sum = (16+31)
		Найти соседей для вертикальной суммы и прибавить к крайнему правому элементу r0sum right_sum,
		а к крайнему левому элементу r1sum прибавить left_sum.
		Так же сохранить крайний правый элемент из r1sum, так как он понадобится далее в вычислениях
		*/

		uint8_t
			left2_sum {},
			right2_sum {},
			left3_sum {},
			right3_sum {},
			__l_temp {},
			__r_temp {};


		// хранит крайний правый элемент второго регистра
		uint8_t save_for_next_iter {};

#ifdef _DEBUG
		auto DEBUG_REGS = [&] (string at_moment) {
			std::printf("regs %s\n", at_moment.c_str());
			print_two_uint(top, "top");
			print_two_uint(mid, "mid");
			print_two_uint(low, "low");
			print_two_uint(r3sum, "rr");
			std::printf("kpe: %lu, kle: %lu, __l: %lu, __r: %lu, save: %lu\n", left2_sum, right2_sum, __l_temp, __r_temp, save_for_next_iter);
			std::printf("\n");
		};

		auto _DEBUG_REG = [] (__m128i reg, string msg) {
			std::printf("reg: %s\n", msg.c_str());
			sseHelperNS::print_uint8(reg, "reg"); printf("\n");
		};

		auto DEBUG_RES = [&to_save] (string at_moment, size_t offset = 0) {
			if (offset > 0) {
				std::cout << "offset " << offset << " ";
			}
			std::cout << "to_save " << at_moment << "\n";
			to_save._debug_print_as_arrays(16);
		};
#else
#define _DEBUG_REG(reg, at_moment) ((void)0)
#define DEBUG_REGS(at_moment) ((void)0)
#define DEBUG_RES(at_moment) ((void)0)
#endif // _DEBUG

		auto calcFirstTwoLines = [&top, &mid, &low, &res_ptr] (uint8_t* rightElement, size_t save_offset) -> __m128i {
			__m128i verticalSum0 = _mm_add_epi8(top[0], mid[0]);
			__m128i verticalSum1 = _mm_add_epi8(top[1], mid[1]);

			uint8_t left_sum = _mm_extract_epi8(verticalSum0, 15);
			uint8_t right_sum = _mm_extract_epi8(verticalSum1, 0);

			*rightElement = _mm_extract_epi8(verticalSum1, 15);

			verticalSum0 = getNeighboursByVertical(verticalSum0); // теперь здесь хранится сумма этого элемента с соседями
			verticalSum1 = getNeighboursByVertical(verticalSum1);

			uint8_t leftValue = _mm_extract_epi8(verticalSum0, 15);
			uint8_t rightValue = _mm_extract_epi8(verticalSum1, 0);

			leftValue += right_sum;
			rightValue += left_sum;

			verticalSum0 = _mm_insert_epi8(verticalSum0, leftValue, 15);	// для крайних элементов вставлена недостающая сумма
			verticalSum1 = _mm_insert_epi8(verticalSum1, rightValue, 0);

			verticalSum0 = _mm_sub_epi8(verticalSum0, top[0]);	// теперь тут хранится результат для первой строчки

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), verticalSum0);

			return verticalSum1;
		};

		auto calcTwoLines = [&top, &mid, &low, &res_ptr] (__m128i leftVerticalResWithoutRightNeighbour, uint8_t* rightElement, size_t save_offset) -> __m128i {
			__m128i verticalSum1 = _mm_add_epi8(top[1], mid[1]);

			uint8_t left_sum = *rightElement;
			
			uint8_t leftValue = _mm_extract_epi8(leftVerticalResWithoutRightNeighbour, 15);
			uint8_t right_sum = _mm_extract_epi8(verticalSum1, 0);
			*rightElement = _mm_extract_epi8(verticalSum1, 15);

			verticalSum1 = getNeighboursByVertical(verticalSum1);	 // теперь здесь хранится сумма этого элемента с соседями

			uint8_t rightValue = _mm_extract_epi8(verticalSum1, 0);

			leftValue += right_sum;
			rightValue += left_sum;

			leftVerticalResWithoutRightNeighbour = _mm_insert_epi8(leftVerticalResWithoutRightNeighbour, leftValue, 15);	// для крайних элементов вставлена недостающая сумма
			verticalSum1 = _mm_insert_epi8(verticalSum1, rightValue, 0);

			leftVerticalResWithoutRightNeighbour = _mm_sub_epi8(leftVerticalResWithoutRightNeighbour, top[0]);

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), leftVerticalResWithoutRightNeighbour);

			return verticalSum1;
		};

		auto calcTwoLastLines = [&top, &mid, &low, &res_ptr, &remainder] (size_t save_offset) {
			__m128i verticalSum = _mm_add_epi8(top[1], mid[1]);

			verticalSum = getNeighboursByVertical(verticalSum);	 // теперь здесь хранится сумма этого элемента с соседями
			verticalSum = _mm_sub_epi8(verticalSum, top[1]);

			uint8_t buffer[16];
			_mm_store_si128(reinterpret_cast<__m128i*>(buffer), verticalSum);

			memcpy(res_ptr + save_offset + remainder, buffer + remainder, 16 - remainder);
		};

		auto calcFirstThreeLines = [&top, &mid, &low, &res_ptr] (uint8_t* rightElement, size_t save_offset) -> __m128i {
			__m128i verticalSum0, verticalSum1;
			verticalSum0 = justSum(top[0], mid[0], low[0]);
			verticalSum1 = justSum(top[1], mid[1], low[1]);

			uint8_t left_sum = _mm_extract_epi8(verticalSum0, 15);
			uint8_t right_sum = _mm_extract_epi8(verticalSum1, 0);

			*rightElement = _mm_extract_epi8(verticalSum1, 15);

			verticalSum0 = getNeighboursByVertical(verticalSum0);			// теперь здесь хранится сумма этого элемента с соседями
			verticalSum1 = getNeighboursByVertical(verticalSum1);

			uint8_t leftValue = _mm_extract_epi8(verticalSum0, 15);
			uint8_t rightValue = _mm_extract_epi8(verticalSum1, 0);

			leftValue += right_sum;
			rightValue += left_sum;

			verticalSum0 = _mm_insert_epi8(verticalSum0, leftValue, 15);	// для крайних элементов вставлена недостающая сумма
			verticalSum1 = _mm_insert_epi8(verticalSum1, rightValue, 0);

			verticalSum0 = _mm_sub_epi8(verticalSum0, mid[0]);	// теперь тут хранится результат для первой строчки

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), verticalSum0);

			return verticalSum1;
		};

		auto calcThreeLines = [&top, &mid, &low, &res_ptr] (__m128i leftVerticalResWithoutRightNeighbour, uint8_t* rightElement, size_t save_offset) -> __m128i {
			__m128i verticalSum1 = justSum(top[1], mid[1], low[1]);

			uint8_t left_sum = *rightElement;

			uint8_t leftValue = _mm_extract_epi8(leftVerticalResWithoutRightNeighbour, 15);
			uint8_t right_sum = _mm_extract_epi8(verticalSum1, 0);
			*rightElement = _mm_extract_epi8(verticalSum1, 15);

			verticalSum1 = getNeighboursByVertical(verticalSum1);

			uint8_t rightValue = _mm_extract_epi8(verticalSum1, 0);

			leftValue += right_sum;
			rightValue += left_sum;

			leftVerticalResWithoutRightNeighbour = _mm_insert_epi8(leftVerticalResWithoutRightNeighbour, leftValue, 15);	// для крайних элементов вставлена недостающая сумма
			verticalSum1 = _mm_insert_epi8(verticalSum1, rightValue, 0);

			leftVerticalResWithoutRightNeighbour = _mm_sub_epi8(leftVerticalResWithoutRightNeighbour, mid[0]);	// теперь тут хранится результат для первой строчки

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), leftVerticalResWithoutRightNeighbour);

			return verticalSum1;
		};

		auto calcThreeLastLines = [&top, &mid, &low, &res_ptr, &remainder] (size_t save_offset) {
			__m128i verticalSum = justSum(top[1], mid[1], low[1]);

			verticalSum = getNeighboursByVertical(verticalSum);
			verticalSum = _mm_sub_epi8(verticalSum, mid[1]);

			uint8_t buffer[16];
			_mm_store_si128(reinterpret_cast<__m128i*>(buffer), verticalSum);

			memcpy(res_ptr + save_offset + remainder, buffer + remainder, 16 - remainder);
		};

		size_t offset = 0;

		const size_t total_blocks_count = blocks_count + bool(remainder);

		/*
		зарисовка работы алгоритма:
		          первый блок           |         средние блоки			   | последний блок |
		0  1 ... 14 15 | 16 17 .. 30 31 | 32 33 ... 46 47 | 48 49 .. 62 63 | 63 65 .. 77 78 |
		из этого следует, что первый блок всегда обрабатывается отдельно. последний может излишне
		записать в ненужные блоки, но результаты вычислений не должны исказиться из-за записи последующих решений

		для последней строки последний блок всегда обрабатывается отдельно!
		*/

		// первый блок
		top[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr));
		top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + 16));
		
		mid[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + width));
		mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + width + 16));
		
		low[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + 2 * width));
		low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + 2 * width + 16));

		uint8_t vertical2sum {}, vertical3sum {};
		auto saved2VerticalSum = calcFirstTwoLines(&vertical2sum, 0);
		auto saved3VerticalSum = calcFirstThreeLines(&vertical3sum, width);
		
		//DEBUG_RES("after first sums");

		offset += 32;

		// средние строки
		for (size_t i = 1; i < blocks_count; i++, offset += 16) {
			top[0] = top[1];
			mid[0] = mid[1];
			low[0] = low[1];

			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + offset));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + offset + width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + offset + 2 * width));

			saved2VerticalSum = calcTwoLines(saved2VerticalSum, &vertical2sum, offset - 16);
			saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, width + offset - 16);
		}

		//DEBUG_RES("after middle sums");

		if (remainder > 0) {
			const size_t _internal_offset = width - 16;
			T* ptr = obj_ptr + _internal_offset;

			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

			calcTwoLastLines(_internal_offset);
			calcThreeLastLines(_internal_offset + width);
		}
		DEBUG_RES("after last row first sums");

		//return;

		offset = 0;

		for (size_t y = 1, y_offset = y * width; y < height - 2; y++, y_offset += width) {
			T* ptr = obj_ptr + y_offset;
			top[0] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 16));

			mid[0] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width + 16));

			low[0] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width + 16));

			saved3VerticalSum = calcFirstThreeLines(&vertical3sum, y_offset + width);
			DEBUG_RES("after first sum second line", y_offset + width);
			offset += 32;

			for (size_t i = 1; i < blocks_count; i++, offset += 16) {
				ptr = ptr + offset;
				top[0] = top[1];
				mid[0] = mid[1];
				low[0] = low[1];

				top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
				mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
				low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

				const size_t _offset = y_offset + width + offset - 16;

				saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, _offset);
				DEBUG_RES("after second sum second line", _offset);
			}
			
			if (remainder == 0) continue;

			const size_t _internal_offset = remainder;
			ptr += _internal_offset;

			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

			//calcThreeLastLines(_internal_offset);

			offset += _internal_offset;

			//DEBUG_RES("after last sum second line", offset);

			
		}
	}
};
