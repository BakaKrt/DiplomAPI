module;
#include <immintrin.h>

export module AvxBufferedRealization ;

import std;
import totalRealizationBase;

import MasksCreator;

import sseHelper;

using std::string;
using std::array;

using std::bitset;

using std::memcpy;
using std::printf;

using namespace sseHelperNS;

export class AvxBufferedRealization : public TotalRealizationBase<AvxBufferedRealization > {
private:
	inline static __m256i justSum(__m256i r0, __m256i r1, __m256i r2) noexcept {
		// никак не получается выйти в паралеллизм на уровне инструкций, т.к. результат одного требуется для результата следующей операции
		__m256i result;
		result = _mm256_add_epi8(r0, r1);
		result = _mm256_add_epi8(result, r2);

		return result;
	}

	/// <summary>
	/// Результат суммы соседей слева и справа сохраняется в регистр r0
	/// </summary>
	/// <param name="r0"></param>
	/// <param name="r1"></param>
	/// <param name="r2"></param>
	inline static void justSum(__m256i* r0, __m256i r1, __m256i r2) noexcept {
		// никак не получается выйти в паралеллизм на уровне инструкций, т.к. результат одного требуется для результата следующей операции
		*r0 = _mm256_add_epi8(*r0, r1);
		*r0 = _mm256_add_epi8(*r0, r2);
	}

	inline static __m256i getNeighboursByVertical(__m256i r0) noexcept {
		return
			_mm256_add_epi8(r0,
				_mm256_add_epi8(
					_mm256_srli_si256(r0, 1), _mm256_slli_si256(r0, 1)
				)
			);
	}

	bitset<9> ruleB {};
	bitset<9> ruleS {};
	static constexpr size_t windowSize = 8;
public:
	AvxBufferedRealization() {
		name = "avx buf";
		ruleB.set(3);
		ruleS.set(2).set(3);
	}

	inline const string getNameImpl() const {
		return name;
	}

	/// <summary>
	/// Реализует сумму соседей. Итерируется сначала по горизонтали, а потом по вертикали. Не поддерживает массивы шириной меньше 16
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="object"></param>
	/// <param name="to_save"></param>
	__declspec(noinline) void runImpl(Flat2DArray<uint8_t>& object, Flat2DArray<uint8_t>& to_save) const noexcept {
		using T = uint8_t;
		constexpr size_t REGISTER_SIZE = 32; // размер окна для AVX = 32 элементов типа uint8_t

		const size_t width = object.width();
		const size_t height = object.height();

		auto res_ptr = to_save.data();
		auto obj_ptr = object.data();

		// индекс начала нижней строки (то есть 1-ой строки) [0-ая это верхняя]
		const size_t mid_index = width;

		// количество блоков, которые полностью помещаются в ширину
		const size_t blocks_count = mid_index / REGISTER_SIZE;

		// количество блоков, которые не полностью помещаются в ширину. Если ширина не кратна SSE_SIZE, то последний блок будет обрабатываться отдельно
		const size_t remainder = width % REGISTER_SIZE;



		array<__m256i, 2> top {};
		array<__m256i, 2> mid {};
		array<__m256i, 2> low {};

		/*
		граница двух регистров:
		r00: ... 14 15 | r01: 16 17 ...
		r10: ... 29 30 | r11: 31 32 ...

		так как перекрытия нет, нужно сначала вертикально сложить r0sum = (r00, r10) и r1sum = (r01, r11).
		Взять крайние элементы, в данном случае left_sum = (15+30) и right_sum = (16+31)
		Найти соседей для вертикальной суммы и прибавить к крайним левому и правому элементам r0sum и r1sum соответственно.
		Так же сохранить крайний правый элемент из r1sum, так как он понадобится далее в вычислениях
		*/


#define _DEBUG_AVX_HORIZONTAL 0
#if defined(_DEBUG) && _DEBUG_AVX_HORIZONTAL == 1
		auto DEBUG_REGS = [&] (string at_moment) {
			//std::printf("regs %s\n", at_moment.c_str());
			//print_two_uint(top, "top");
			//print_two_uint(mid, "mid");
			//print_two_uint(low, "low");
			//std::printf("\n");
			};

		auto _DEBUG_REG = [] (__m256i reg, string msg) {
			//std::printf("reg: %s\n", msg.c_str());
			//sseHelperNS::print_uint8(reg, "reg"); printf("\n");
			};

		auto DEBUG_RES = [&to_save] (string at_moment, size_t offset = 0) {
			if (offset > 0) {
				std::cout << "x_offset " << offset << " ";
			}
			std::cout << "to_save " << at_moment << "\n";
			to_save._debug_print_as_arrays(REGISTER_SIZE);
			};
#else
#define _DEBUG_REG(reg, at_moment) ((void)0)
#define DEBUG_REGS(at_moment) ((void)0)
#define DEBUG_RES(at_moment, offset) ((void)0)
#endif // _DEBUG
#pragma region lambdas

		auto saveRes = [this, &obj_ptr, &res_ptr] (__m256i& from, size_t save_offset) noexcept {
			alignas(32) array<T, REGISTER_SIZE> sums {};
			_mm256_store_si256(reinterpret_cast<__m256i*>(sums.data()), from);

			constexpr size_t BLOCK_SIZE = 8;
			constexpr size_t NUM_BLOCKS = REGISTER_SIZE / BLOCK_SIZE; // 4

			for (size_t block_idx = 0; block_idx < NUM_BLOCKS; ++block_idx) {
				size_t base_offset = save_offset + (block_idx * BLOCK_SIZE);

				// Локальный буфер для результатов текущего 8-байтного блока
				alignas(8) array<T, BLOCK_SIZE> res_block {};

				for (size_t i = 0; i < BLOCK_SIZE; ++i) {
					bool alive = obj_ptr[base_offset + i];
					T neighboursCount = sums[block_idx * BLOCK_SIZE + i];

					bool b_contains = ruleB.test(neighboursCount);
					bool s_contains = ruleS.test(neighboursCount);

					res_block[i] = alive ? s_contains : b_contains;
				}
				memcpy(res_ptr + base_offset, res_block.data(), BLOCK_SIZE);
			}
		};


		auto saveResNotFit = [this, &obj_ptr, &res_ptr, &remainder] (__m256i& from, size_t save_offset) noexcept {
			alignas(REGISTER_SIZE) array<T, REGISTER_SIZE> sums {};
			_mm256_store_si256(reinterpret_cast<__m256i*>(sums.data()), from);

			constexpr size_t BLOCK_SIZE = 8;

			size_t remaining_bytes = remainder % BLOCK_SIZE;

			if (remaining_bytes > 0) {
				size_t base_offset = save_offset;
				alignas(8) array<T, BLOCK_SIZE> res_block {};

				for (size_t i = 0; i < remaining_bytes; ++i) {
					size_t sum_index = i;

					bool alive = obj_ptr[base_offset + i];
					T neighboursCount = sums[sum_index];

					bool b_contains = ruleB.test(neighboursCount);
					bool s_contains = ruleS.test(neighboursCount);

					res_block[i] = alive ? s_contains : b_contains;
				}
				memcpy(res_ptr + base_offset, res_block.data(), remaining_bytes);
			}
		};

		auto calcFirstTwoLines = [&top, &mid, &low, &res_ptr, saveRes](__m256i* rightElement, size_t save_offset) -> __m256i {
			__m256i verticalSum0 = _mm256_add_epi8(top[0], mid[0]);
			__m256i verticalSum1 = _mm256_add_epi8(top[1], mid[1]);

			// тут загвоздка в реализации, проверить сайт интел, потому что результат неправильный
			__m256i cross_sum = _mm256_alignr_epi8(verticalSum1, verticalSum0, REGISTER_SIZE - 1);
			cross_sum = _mm256_slli_si256(cross_sum, REGISTER_SIZE - 2);

			const __m256i mask0 = _mm256_setr_epi8(REGISTER_SIZE - 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			const __m256i mask1 = _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, REGISTER_SIZE - 1);

			const __m256i leftVElem = _mm256_shuffle_epi8(cross_sum, mask0);
			const __m256i rightVElem = _mm256_shuffle_epi8(cross_sum, mask1);

			__m256i neighbours0 = getNeighboursByVertical(verticalSum0);
			__m256i neighbours1 = getNeighboursByVertical(verticalSum1);

			neighbours0 = _mm256_add_epi8(neighbours0, rightVElem);
			neighbours1 = _mm256_add_epi8(neighbours1, leftVElem);

			*rightElement = verticalSum1;

			neighbours0 = _mm256_sub_epi8(neighbours0, top[0]);	// теперь тут хранится результат для первой строчки

			saveRes(neighbours0, save_offset);
			//_mm256_store_si256(reinterpret_cast<__m256i*>(res_ptr + save_offset), neighbours0);

			return neighbours1;
		};

		auto calcTwoLines = [&top, &mid, &low, &res_ptr, saveRes](__m256i leftVerticalResWithoutRightNeighbour, __m256i* leftVElem, size_t save_offset) noexcept -> __m256i {
			__m256i verticalSum1 = _mm256_add_epi8(top[1], mid[1]);
			__m256i neighbours1 = getNeighboursByVertical(verticalSum1);

			__m256i leftElementRightElements = _mm256_srli_si256(*leftVElem, REGISTER_SIZE - 1);

			__m256i rightVElem = _mm256_slli_si256(verticalSum1, REGISTER_SIZE - 1);
			*leftVElem = verticalSum1;

			neighbours1 = _mm256_add_epi8(neighbours1, leftElementRightElements);

			__m256i leftTotalResult = _mm256_add_epi8(leftVerticalResWithoutRightNeighbour, rightVElem);
			leftTotalResult = _mm256_sub_epi8(leftTotalResult, top[0]);

			saveRes(leftTotalResult, save_offset);
			//_mm256_store_si256(reinterpret_cast<__m256i*>(res_ptr + save_offset), leftTotalResult);

			return neighbours1;
		};

		auto calcTwoLastNotFitLines = [&top, &mid, &low, &res_ptr, &remainder, saveResNotFit] (size_t save_offset) {
			__m256i verticalSum = _mm256_add_epi8(top[1], mid[1]);

			verticalSum = getNeighboursByVertical(verticalSum);	 // теперь здесь хранится сумма этого элемента с соседями
			verticalSum = _mm256_sub_epi8(verticalSum, top[1]);

			saveResNotFit(verticalSum, save_offset);

			//uint8_t buffer[REGISTER_SIZE] {};
			//_mm256_store_si256(reinterpret_cast<__m256i*>(buffer), verticalSum);

			//memcpy(res_ptr + save_offset + remainder, buffer + remainder, REGISTER_SIZE - remainder);
			};

		auto calcFirstThreeLines = [&top, &mid, &low, &res_ptr, saveRes](__m256i* rightVSumElem, size_t save_offset) noexcept -> __m256i {
			__m256i verticalSum0 = justSum(top[0], mid[0], low[0]);
			__m256i verticalSum1 = justSum(top[1], mid[1], low[1]);

			__m256i cross_sum = _mm256_alignr_epi8(verticalSum1, verticalSum0, REGISTER_SIZE - 1);
			cross_sum = _mm256_slli_si256(cross_sum, REGISTER_SIZE - 2);

			const __m256i mask0 = _mm256_setr_epi8(REGISTER_SIZE - 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			const __m256i mask1 = _mm256_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, REGISTER_SIZE - 1);


			const __m256i leftVElem = _mm256_shuffle_epi8(cross_sum, mask0);
			const __m256i rightVElem = _mm256_shuffle_epi8(cross_sum, mask1);

			__m256i neighbours0 = getNeighboursByVertical(verticalSum0);
			__m256i neighbours1 = getNeighboursByVertical(verticalSum1);

			neighbours0 = _mm256_add_epi8(neighbours0, rightVElem);
			neighbours1 = _mm256_add_epi8(neighbours1, leftVElem);

			*rightVSumElem = verticalSum1;

			neighbours0 = _mm256_sub_epi8(neighbours0, mid[0]);	// теперь тут хранится результат для первой строчки

			saveRes(neighbours0, save_offset);
			//_mm256_store_si256(reinterpret_cast<__m256i*>(res_ptr + save_offset), neighbours0);

			return neighbours1;
		};

		auto calcThreeLines = [&top, &mid, &low, &res_ptr, saveRes](__m256i leftVerticalResWithoutRightNeighbour, __m256i* leftVElem, size_t save_offset) -> __m256i {
			__m256i verticalSum1 = justSum(top[1], mid[1], low[1]);
			__m256i neighbours1 = getNeighboursByVertical(verticalSum1);

			__m256i leftElementRightElements = _mm256_srli_si256(*leftVElem, REGISTER_SIZE - 1);

			__m256i rightVElem = _mm256_slli_si256(verticalSum1, REGISTER_SIZE - 1);
			*leftVElem = verticalSum1;

			neighbours1 = _mm256_add_epi8(neighbours1, leftElementRightElements);

			__m256i leftTotalResult = _mm256_add_epi8(leftVerticalResWithoutRightNeighbour, rightVElem);
			leftTotalResult = _mm256_sub_epi8(leftTotalResult, mid[0]);

			saveRes(leftTotalResult, save_offset);
			//_mm256_store_si256(reinterpret_cast<__m256i*>(res_ptr + save_offset), leftTotalResult);

			return neighbours1;
		};

		auto calcThreeLastNotFitLines = [&top, &mid, &low, &res_ptr, &remainder, saveResNotFit] (size_t save_offset) {
			__m256i verticalSum = justSum(top[1], mid[1], low[1]);

			verticalSum = getNeighboursByVertical(verticalSum);
			verticalSum = _mm256_sub_epi8(verticalSum, mid[1]);

			saveResNotFit(verticalSum, save_offset);

			//uint8_t buffer[REGISTER_SIZE];
			//_mm256_store_si256(reinterpret_cast<__m256i*>(buffer), verticalSum);

			//memcpy(res_ptr + save_offset + remainder, buffer + remainder, REGISTER_SIZE - remainder);
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
		top[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr));
		top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + REGISTER_SIZE));

		mid[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + width));
		mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + width + REGISTER_SIZE));

		low[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + 2 * width));
		low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + 2 * width + REGISTER_SIZE));

		__m256i vertical2sum {}, vertical3sum {};
		auto saved2VerticalSum = calcFirstTwoLines(&vertical2sum, 0);
		auto saved3VerticalSum = calcFirstThreeLines(&vertical3sum, width);

		//DEBUG_RES("after first sums");

		x_offset += REGISTER_SIZE;

		// средние строки
		for (size_t i = 1; i < blocks_count; i++, x_offset += REGISTER_SIZE) {
			T* ptr = obj_ptr + x_offset + REGISTER_SIZE;

			top[0] = top[1];
			mid[0] = mid[1];
			low[0] = low[1];

			top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr));
			mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width));
			low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width));

			saved2VerticalSum = calcTwoLines(saved2VerticalSum, &vertical2sum, x_offset);
			saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, width + x_offset);
		}

		DEBUG_RES("after middle sums", x_offset);

		if (remainder > 0) {
			x_offset = x_offset - REGISTER_SIZE + remainder;

			T* ptr = obj_ptr + x_offset;

			top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr));
			mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width));
			low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width));

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

			top[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr));
			top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + REGISTER_SIZE));

			mid[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + width));
			mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + width + REGISTER_SIZE));

			low[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + 2 * width));
			low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(y_ptr + 2 * width + REGISTER_SIZE));

			saved3VerticalSum = calcFirstThreeLines(&vertical3sum, y_offset + width);
			DEBUG_RES("after first sum second line", y_offset + width);

			x_offset += REGISTER_SIZE;

			// средние блоки
			T* ptr = nullptr;
			for (size_t i = 1; i < blocks_count; i++, x_offset += REGISTER_SIZE) {
				ptr = y_ptr + x_offset + REGISTER_SIZE;
				top[0] = top[1];
				mid[0] = mid[1];
				low[0] = low[1];

				top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr));
				mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width));
				low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width));

				saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, y_offset + x_offset + width);
				DEBUG_RES("after second sum second line", x_offset);
			}

			if (remainder == 0) continue;

			// последние блоки
			x_offset = x_offset - REGISTER_SIZE + remainder;
			ptr = y_ptr + x_offset;

			top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr));
			mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width));
			low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width));

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
		low[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr));
		low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + REGISTER_SIZE));

		mid[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width));
		mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width + REGISTER_SIZE));

		top[0] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width));
		top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width + REGISTER_SIZE));

		saved2VerticalSum = calcFirstTwoLines(&vertical2sum, y_offset + 2 * width);
		saved3VerticalSum = calcFirstThreeLines(&vertical3sum, y_offset + width);

		DEBUG_RES("after first sums", x_offset);

		x_offset = REGISTER_SIZE;

		// средние строки
		for (size_t i = 1; i < blocks_count; i++, x_offset += REGISTER_SIZE) {
			ptr = obj_ptr + y_offset + x_offset + REGISTER_SIZE;

			low[0] = low[1];
			mid[0] = mid[1];
			top[0] = top[1];

			low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr));
			mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width));
			top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width));

			saved2VerticalSum = calcTwoLines(saved2VerticalSum, &vertical2sum, y_offset + x_offset + 2 * width);
			saved3VerticalSum = calcThreeLines(saved3VerticalSum, &vertical3sum, y_offset + x_offset + width);
		}

		DEBUG_RES("after middle sums", x_offset);

		if (remainder == 0) return;

		x_offset = x_offset - REGISTER_SIZE + remainder;

		ptr = obj_ptr + y_offset + x_offset;

		low[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr));
		mid[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + width));
		top[1] = _mm256_load_si256(reinterpret_cast<__m256i*>(ptr + 2 * width));

		calcTwoLastNotFitLines(y_offset + x_offset + 2 * width);
		calcThreeLastNotFitLines(y_offset + x_offset + width);

		DEBUG_RES("after last row first sums", x_offset);
#pragma endregion
	}
};
