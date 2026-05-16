module;
#include <emmintrin.h>
#include <tmmintrin.h>
#include <cstdint>

export module sse_horizontal;

import std;
import sumRealizationBase;

import sseHelper;

using std::string;
using std::array;

using std::memcpy;
using std::printf;

using namespace sseHelperNS;

export class SSEv2Sum : public SumRealizationBase<SSEv2Sum> {
public:
	SSEv2Sum() { name = "sse v2"; }

	inline const string getName_impl() const {
		return name;
	}


	inline static __m128i justSum(__m128i r0, __m128i r1, __m128i r2) noexcept {
		// никак не получается выйти в паралеллизм на уровне инструкций, т.к. результат одного требуется для результата следующей операции
		__m128i result = _mm_add_epi8(r0, r1);
		return _mm_add_epi8(result, r2);
	}

	/// <summary>
	/// Результат суммы соседей слева и справа сохраняется в регистр r0
	/// </summary>
	/// <param name="r0"></param>
	/// <param name="r1"></param>
	/// <param name="r2"></param>
	inline static void justSum(__m128i* r0, __m128i r1, __m128i r2) noexcept {
		*r0 = _mm_add_epi8(*r0, r1);
		*r0 = _mm_add_epi8(*r0, r2);
	}

	/// <summary>
	/// Задержка: 1 + 2 = 3
	/// </summary>
	/// <param name="r0"></param>
	/// <returns></returns>
	inline static __m128i getNeighboursByVertical(__m128i r0) noexcept {
		// параллелизм на уровне инструкций (операции не зависят друг от друга и процессор может выполнить их одновременно)
		__m128i left = _mm_slli_si128(r0, 1);
		__m128i right = _mm_srli_si128(r0, 1);
		__m128i neighbors = _mm_add_epi8(left, right);
		return _mm_add_epi8(r0, neighbors);
	}


	/// <summary>
	/// Реализует сумму соседей. Итерируется сначала по горизонтали, а потом по вертикали. Не поддерживает массивы шириной меньше 16
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="object"></param>
	/// <param name="to_save"></param>
	template<typename T> requires allowed_type<T>
	__declspec(noinline) void test_runImpl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		constexpr size_t WINDOW_SIZE = 16; // размер окна для SSE = 16 элементов типа uint8_t

		const size_t width  = object.width();
		const size_t height = object.height();

		auto res_ptr = to_save.data();
		auto obj_ptr = object.data();

		// индекс начала нижней строки (то есть 1-ой строки) [0-ая это верхняя]
		const size_t mid_index = width;

		// количество блоков, которые полностью помещаются в ширину
		const size_t blocks_count = mid_index / WINDOW_SIZE;

		// количество блоков, которые не полностью помещаются в ширину. Если ширина не кратна SSE_SIZE, то последний блок будет обрабатываться отдельно
		const size_t remainder = width % WINDOW_SIZE;

		

		array<__m128i, 2> top {};
		array<__m128i, 2> mid {};
		array<__m128i, 2> low {};

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


#define _DEBUG_SSE_HORIZONTAL 0
#if defined(_DEBUG) && _DEBUG_SSE_HORIZONTAL == 1
		auto DEBUG_REGS = [&] (string at_moment) {
			std::printf("regs %s\n", at_moment.c_str());
			print_two_uint(top, "top");
			print_two_uint(mid, "mid");
			print_two_uint(low, "low");
			std::printf("\n");
		};

		auto _DEBUG_REG = [] (__m128i reg, string msg) {
			std::printf("reg: %s\n", msg.c_str());
			sseHelperNS::print_uint8(reg, "reg"); printf("\n");
		};

		auto DEBUG_RES = [&to_save] (string at_moment, size_t offset = 0) {
			if (offset > 0) {
				std::cout << "x_offset " << offset << " ";
			}
			std::cout << "to_save " << at_moment << "\n";
			to_save._debug_print_as_arrays(WINDOW_SIZE);
		};
#else
#define _DEBUG_REG(reg, at_moment) ((void)0)
#define DEBUG_REGS(at_moment) ((void)0)
#define DEBUG_RES(at_moment, offset) ((void)0)
#endif // _DEBUG
#pragma region lambdas
		auto calcFirstTwoLines = [&top, &mid, &low, &res_ptr] (__m128i* rightElement, size_t save_offset) -> __m128i {
			__m128i verticalSum0 = _mm_add_epi8(top[0], mid[0]);
			__m128i verticalSum1 = _mm_add_epi8(top[1], mid[1]);

			__m128i cross_sum = _mm_alignr_epi8(verticalSum1, verticalSum0, WINDOW_SIZE - 1);
			cross_sum = _mm_slli_si128(cross_sum, WINDOW_SIZE - 2);

			const __m128i mask0 = _mm_setr_epi8(WINDOW_SIZE - 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0);
			const __m128i mask1 = _mm_setr_epi8( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, WINDOW_SIZE - 1);

			const __m128i leftVElem = _mm_shuffle_epi8(cross_sum, mask0);
			const __m128i rightVElem = _mm_shuffle_epi8(cross_sum, mask1);

			__m128i neighbours0 = getNeighboursByVertical(verticalSum0);
			__m128i neighbours1 = getNeighboursByVertical(verticalSum1);

			neighbours0 = _mm_add_epi8(neighbours0, rightVElem);
			neighbours1 = _mm_add_epi8(neighbours1, leftVElem);

			*rightElement = verticalSum1;

			neighbours0 = _mm_sub_epi8(neighbours0, top[0]);	// теперь тут хранится результат для первой строчки

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), neighbours0);

			return neighbours1;
		};

		auto calcTwoLines = [&top, &mid, &low, &res_ptr] (__m128i leftVerticalResWithoutRightNeighbour, __m128i* leftVElem, size_t save_offset) noexcept -> __m128i {
			__m128i verticalSum1 = _mm_add_epi8(top[1], mid[1]);
			__m128i neighbours1 = getNeighboursByVertical(verticalSum1);

			__m128i leftElementRightElements = _mm_srli_si128(*leftVElem, WINDOW_SIZE - 1);

			__m128i rightVElem = _mm_slli_si128(verticalSum1, WINDOW_SIZE - 1);
			*leftVElem = verticalSum1;

			neighbours1 = _mm_add_epi8(neighbours1, leftElementRightElements);

			__m128i leftTotalResult = _mm_add_epi8(leftVerticalResWithoutRightNeighbour, rightVElem);
			leftTotalResult = _mm_sub_epi8(leftTotalResult, top[0]);
			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), leftTotalResult);
				
			return neighbours1;
		};

		auto calcTwoLastNotFitLines = [&top, &mid, &low, &res_ptr, &remainder] (size_t save_offset) {
			__m128i verticalSum = _mm_add_epi8(top[1], mid[1]);

			verticalSum = getNeighboursByVertical(verticalSum);	 // теперь здесь хранится сумма этого элемента с соседями
			verticalSum = _mm_sub_epi8(verticalSum, top[1]);

			uint8_t buffer[WINDOW_SIZE] {};
			_mm_store_si128(reinterpret_cast<__m128i*>(buffer), verticalSum);

			memcpy(res_ptr + save_offset + remainder, buffer + remainder, WINDOW_SIZE - remainder);
		};

		auto calcFirstThreeLines = [&top, &mid, &low, &res_ptr] (__m128i* rightVSumElem, size_t save_offset) noexcept -> __m128i {
			__m128i verticalSum0 = justSum(top[0], mid[0], low[0]);
			__m128i verticalSum1 = justSum(top[1], mid[1], low[1]);

			__m128i cross_sum = _mm_alignr_epi8(verticalSum1, verticalSum0, WINDOW_SIZE - 1);
			cross_sum = _mm_slli_si128(cross_sum, WINDOW_SIZE - 2);

			const __m128i mask0 = _mm_setr_epi8(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			const __m128i mask1 = _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15);

			const __m128i leftVElem = _mm_shuffle_epi8(cross_sum, mask0);
			const __m128i rightVElem = _mm_shuffle_epi8(cross_sum, mask1);

			__m128i neighbours0 = getNeighboursByVertical(verticalSum0);
			__m128i neighbours1 = getNeighboursByVertical(verticalSum1);

			neighbours0 = _mm_add_epi8(neighbours0, rightVElem);
			neighbours1 = _mm_add_epi8(neighbours1, leftVElem);

			*rightVSumElem = verticalSum1;

			neighbours0 = _mm_sub_epi8(neighbours0, mid[0]);	// теперь тут хранится результат для первой строчки

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), neighbours0);

			return neighbours1;
		};

		auto calcThreeLines = [&top, &mid, &low, &res_ptr] (__m128i leftVerticalResWithoutRightNeighbour, __m128i* leftVElem, size_t save_offset) -> __m128i {
			__m128i verticalSum1 = justSum(top[1], mid[1], low[1]);
			__m128i neighbours1 = getNeighboursByVertical(verticalSum1);

			__m128i leftElementRightElements = _mm_srli_si128(*leftVElem, WINDOW_SIZE - 1);

			__m128i rightVElem = _mm_slli_si128(verticalSum1, WINDOW_SIZE - 1);
			*leftVElem = verticalSum1;

			neighbours1 = _mm_add_epi8(neighbours1, leftElementRightElements);

			__m128i leftTotalResult = _mm_add_epi8(leftVerticalResWithoutRightNeighbour, rightVElem);
			leftTotalResult = _mm_sub_epi8(leftTotalResult, mid[0]);
			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + save_offset), leftTotalResult);

			return neighbours1;
		};

		auto calcThreeLastNotFitLines = [&top, &mid, &low, &res_ptr, &remainder] (size_t save_offset) {
			__m128i verticalSum = justSum(top[1], mid[1], low[1]);

			verticalSum = getNeighboursByVertical(verticalSum);
			verticalSum = _mm_sub_epi8(verticalSum, mid[1]);

			uint8_t buffer[WINDOW_SIZE];
			_mm_store_si128(reinterpret_cast<__m128i*>(buffer), verticalSum);

			memcpy(res_ptr + save_offset + remainder, buffer + remainder, WINDOW_SIZE - remainder);
		};
#pragma endregion

		size_t x_offset = 0;

		const size_t total_blocks_count = blocks_count + bool(remainder);

		/*
		зарисовка работы алгоритма:
		          первый блок           |         средние блоки			   | последний блок |
		0  1 ... 14 15 | 16 17 .. 30 31 | 32 33 ... 46 47 | 48 49 .. 62 63 | 63 65 .. 77 78 |
		из этого следует, что первый блок всегда обрабатывается отдельно. последний может излишне
		записать в ненужные блоки, но результаты вычислений не должны исказиться из-за записи последующих решений

		для последней строки последний блок всегда обрабатывается отдельно!
		*/

		// первый блок верхних строк
#pragma region first_line
		T* y_ptr = obj_ptr;
		top[0] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr));
		top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + WINDOW_SIZE));
		
		mid[0] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + width));
		mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + width + WINDOW_SIZE));
		
		low[0] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + 2 * width));
		low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + 2 * width + WINDOW_SIZE));

		__m128i vertical2sum, vertical3sum;
		auto saved2VerticalSum = calcFirstTwoLines(&vertical2sum, 0);
		auto saved3VerticalSum = calcFirstThreeLines(&vertical3sum, width);
		
		//DEBUG_RES("after first sums");

		x_offset += WINDOW_SIZE;

		// средние строки
		for (size_t i = 1; i < blocks_count; i++, x_offset += WINDOW_SIZE) {
			T* ptr = obj_ptr + x_offset + WINDOW_SIZE;

			top[0] = top[1];
			mid[0] = mid[1];
			low[0] = low[1];

			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

			saved2VerticalSum = calcTwoLines(saved2VerticalSum, &vertical2sum, x_offset);
			saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, width + x_offset);
		}

		DEBUG_RES("after middle sums", x_offset);

		if (remainder > 0) {
			x_offset = x_offset - WINDOW_SIZE + remainder;

			T* ptr = obj_ptr + x_offset;

			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

			calcTwoLastNotFitLines(x_offset);
			calcThreeLastNotFitLines(x_offset + width);
		}
		DEBUG_RES("after last row first sums", x_offset);
#pragma endregion
#pragma region mid_lines
		// средние по вертикали строки
		for (size_t y = 1, y_offset = y * width; y < height - 3; y++, y_offset += width) {
			// y_offset указывает на смещение до верхней строки

			x_offset = 0;

			y_ptr = obj_ptr + y_offset;

			top[0] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr));
			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + WINDOW_SIZE));

			mid[0] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + width));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + width + WINDOW_SIZE));

			low[0] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + 2 * width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(y_ptr + 2 * width + WINDOW_SIZE));

			saved3VerticalSum = calcFirstThreeLines(&vertical3sum, y_offset + width);
			DEBUG_RES("after first sum second line", y_offset + width);

			x_offset += WINDOW_SIZE;

			// средние блоки
			T* ptr = nullptr;
			for (size_t i = 1; i < blocks_count; i++, x_offset += WINDOW_SIZE) {
				ptr = y_ptr + x_offset + WINDOW_SIZE;
				top[0] = top[1];
				mid[0] = mid[1];
				low[0] = low[1];

				top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
				mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
				low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

				saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, y_offset + x_offset + width);
				DEBUG_RES("after second sum second line", x_offset);
			}
			
			if (remainder == 0) continue;

			// последние блоки
			x_offset = x_offset - WINDOW_SIZE + remainder;
			ptr = y_ptr + x_offset;
			
			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

			calcThreeLastNotFitLines(y_offset + x_offset + width);
			DEBUG_RES("after last sum second line", x_offset);
		}
#pragma endregion
		//return;
#pragma region last_line
		// последние по вертикали блоки
		size_t y_offset = width * height - 3 * width; // указатель на предпоследнюю строку, то есть первый ряд для последних двух рядов

		y_ptr = obj_ptr + y_offset;
		T* ptr = y_ptr;

		// меняю местами low и top, т.к. тут идёт инверсная логика
		low[0] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
		low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + WINDOW_SIZE));

		mid[0] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
		mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width + WINDOW_SIZE));

		top[0] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));
		top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width + WINDOW_SIZE));

		saved2VerticalSum = calcFirstTwoLines(&vertical2sum, y_offset + 2 * width);
		saved3VerticalSum = calcFirstThreeLines(&vertical3sum, y_offset + width);

		DEBUG_RES("after first sums", x_offset);

		x_offset = WINDOW_SIZE;

		// средние строки
		for (size_t i = 1; i < blocks_count; i++, x_offset += WINDOW_SIZE) {
			ptr = obj_ptr + y_offset + x_offset + WINDOW_SIZE;

			low[0] = low[1];
			mid[0] = mid[1];
			top[0] = top[1];

			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

			saved2VerticalSum = calcTwoLines(saved2VerticalSum, &vertical2sum, y_offset + x_offset + 2 * width);
			saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, y_offset + x_offset + width);
		}

		DEBUG_RES("after middle sums", x_offset);

		if (remainder == 0) return;

		x_offset = x_offset - WINDOW_SIZE + remainder;

		ptr = obj_ptr + y_offset + x_offset;

		low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr));
		mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + width));
		top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(ptr + 2 * width));

		calcTwoLastNotFitLines(y_offset + x_offset + 2 * width);
		calcThreeLastNotFitLines(y_offset + x_offset + width);
		
		DEBUG_RES("after last row first sums", x_offset);
#pragma endregion
	}
};
