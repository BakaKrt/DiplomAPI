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


	/// <summary>
	/// Не самая быстрая реализация, так как идёт итерация по вертикали, а не по горизонтали
	/// Из-за этого скорее всего страдает кеш-локальность данных, что не даёт получить ещё большую скорость выполнения
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

		array<__m128i, window_size> rr {};	// сохраняю сумму top + low

		uint8_t
			__kpe_left {},	// Крайний Правый Элемент (KPE) для левого регистра
			__kle_right {},	// Крайний Левый Элемент (KLE) для правого регистра
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
			print_two_uint(rr, "rr");
			std::printf("kpe: %lu, kle: %lu, __l: %lu, __r: %lu, save: %lu\n", __kpe_left, __kle_right, __l_temp, __r_temp, save_for_next_iter);
			std::printf("\n");
		};

		auto _DEBUG_REG = [] (__m128i reg, string msg) {
			std::printf("reg: %s\n", msg.c_str());
			sseHelperNS::print_uint8(reg, "reg"); printf("\n");
		};

		auto DEBUG_RES = [&to_save] (string at_moment) {
			std::cout << "to_save " << at_moment << "\n" << to_save << "\n";
		};
#else
#define _DEBUG_REG(reg, at_moment) ((void)0)
#define DEBUG_RES(at_moment) ((void)0)
#endif // _DEBUG

		//IntelliSense ругается на вызов функций из sseHelper
		auto justSum = [] (__m128i& acc, __m128i r1, __m128i r2) {
			acc = _mm_add_epi8(acc, r1);
			acc = _mm_add_epi8(acc, r2);
		};

		auto sumNeighbours = [] (__m128i& acc) {
			__m128i left, right;

			left = _mm_srli_si128(acc, 1);
			right = _mm_slli_si128(acc, 1);

			acc = _mm_add_epi8(acc, left);
			acc = _mm_add_epi8(acc, right);
		};

		size_t offset = 0;

		const size_t total_blocks_count = blocks_count + bool(remainder);

		
#pragma region first_three_rows
		
		auto sum_first_three_rows_first_block_and_save = [&] () {
			__m128i temp1 {}, temp2 {};

			// первые 2 строки
			top[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr));
			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + 16));

			mid[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + mid_index));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + mid_index + 16));

			low[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + mid_index * 2));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + mid_index * 2 + 16));

			temp1 = top[0]; temp2 = top[1];

			rr[0] = _mm_add_epi8(top[0], mid[0]);
			rr[1] = _mm_add_epi8(top[1], mid[1]);


			//DEBUG_REGS("on rr = top + mid");

			__kpe_left = _mm_extract_epi8(rr[0], 15);
			__kle_right = _mm_extract_epi8(rr[1], 0);
			// сохраняю вертикальную сумму для первых двух строк
			save_for_next_iter = _mm_extract_epi8(rr[1], 15);

			top[0] = rr[0];
			top[1] = rr[1];

			sumNeighbours(top[0]);
			sumNeighbours(top[1]);

			//DEBUG_REGS("on sum neighbours for top");

			__l_temp = _mm_extract_epi8(top[0], 15);
			__r_temp = _mm_extract_epi8(top[1], 0);

			top[0] = _mm_insert_epi8(top[0], __l_temp + __kle_right, 15);
			top[1] = _mm_insert_epi8(top[1], __r_temp + __kpe_left, 0);
			//DEBUG_REGS("after insert");
				
			top[0] = _mm_sub_epi8(top[0], temp1);
			top[1] = _mm_sub_epi8(top[1], temp2);
			//DEBUG_REGS("after sub and to store");
			

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr), top[0]);

			//std::cout << "calc for three lines\n";

			// первые 3 строки
			rr[0] = _mm_add_epi8(rr[0], low[0]);
			rr[1] = _mm_add_epi8(rr[1], low[1]);
			//DEBUG_REGS("after sum");

			__kpe_left = _mm_extract_epi8(rr[0], 15);
			__kle_right = _mm_extract_epi8(rr[1], 0);

			low[0] = rr[0];
			low[1] = rr[1];

			sumNeighbours(low[0]);
			sumNeighbours(low[1]);
			//DEBUG_REGS("after neighbours for low");

			__l_temp = _mm_extract_epi8(low[0], 15);
			__r_temp = _mm_extract_epi8(low[1], 0);

			low[0] = _mm_insert_epi8(low[0], __l_temp + __kle_right, 15);
			low[1] = _mm_insert_epi8(low[1], __r_temp + __kpe_left, 0);
			//DEBUG_REGS("after insert");

			mid[0] = _mm_sub_epi8(low[0], mid[0]);
			mid[1] = _mm_sub_epi8(low[1], mid[1]);
			//DEBUG_REGS("after sub low = low - mid");

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + width), mid[0]);
		};
			
		sum_first_three_rows_first_block_and_save();

		auto sum_first_three_lines_mid_blocks = [&] (size_t load_from_offset) {
			__m128i temp1 {}, temp2 {};

			top[0] = top[1];
			mid[0] = mid[1];
			low[0] = low[1];
			rr[0] = rr[1];

			// первые 2 строки
			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + load_from_offset));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + mid_index + load_from_offset));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_ptr + mid_index * 2 + load_from_offset));

			temp1 = top[0], temp2 = top[1];

			rr[1] = _mm_add_epi8(top[1], mid[1]);
			DEBUG_REGS("on load for mid objects");
			

			__kpe_left = save_for_next_iter;
			__kle_right = _mm_extract_epi8(rr[1], 0);
			save_for_next_iter = _mm_extract_epi8(rr[1], 15);

			top[1] = rr[1];

			sumNeighbours(top[1]);
			DEBUG_REGS("after sum neighbours for top");

			__l_temp = _mm_extract_epi8(top[0], 15);
			__r_temp = _mm_extract_epi8(top[1], 0);

			top[0] = _mm_insert_epi8(top[0], __l_temp + __kle_right, 15);
			top[1] = _mm_insert_epi8(top[1], __r_temp + __kpe_left, 0);
			DEBUG_REGS("after insert in top");

			//top[0] = _mm_sub_epi8(top[0], temp1);
			top[1] = _mm_sub_epi8(top[1], temp2);
			
			DEBUG_REGS("after sub low = low - mid");

			std::cout << "#######\ncalc for three lines\n#######\n\n";

			// первые 3 строки
			//rr[0] = _mm_add_epi8(rr[0], low[0]);
			rr[1] = _mm_add_epi8(rr[1], low[1]);
			DEBUG_REGS("after sum");

			__kpe_left = _mm_extract_epi8(rr[0], 15);
			__kle_right = _mm_extract_epi8(rr[1], 0);

			//low[0] = rr[0];
			low[1] = rr[1];

			//sumNeighbours(low[0]);
			sumNeighbours(low[1]);
			DEBUG_REGS("after neighbours for low");

			__l_temp = _mm_extract_epi8(low[0], 15);
			__r_temp = _mm_extract_epi8(low[1], 0);

			low[0] = _mm_insert_epi8(low[0], __l_temp + __kle_right, 15);
			low[1] = _mm_insert_epi8(low[1], __r_temp + __kpe_left, 0);
			DEBUG_REGS("after insert");

			//low[0] = _mm_sub_epi8(low[0], mid[0]);
			mid[1] = _mm_sub_epi8(low[1], mid[1]);
			DEBUG_REGS("after sub mid = low - mid");

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + load_from_offset - 16), top[0]);
			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + load_from_offset - 16 + width), mid[0]);
		};

		for (size_t x = 32, i = 2; i < total_blocks_count; x += 16, i++) {
			sum_first_three_lines_mid_blocks(x);
		}

#pragma endregion

		return;

		T* obj_offseted_ptr = nullptr;


		for (size_t y = 1; y < height; y++) {
			obj_offseted_ptr = obj_ptr + (y * width);
#pragma region first_element
			top[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr));
			top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr + 16));

			mid[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr + mid_index));
			mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr + mid_index + 16));

			low[0] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr + mid_index * 2));
			low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr + mid_index * 2 + 16));

			DEBUG_REGS("on load");

			justSum(top[0], mid[0], low[0]);
			justSum(top[1], mid[1], low[1]);

			rr[0] = top[0];
			rr[1] = top[1];

			DEBUG_REGS("after vertical sum");

			sumNeighbours(top[0]);
			sumNeighbours(top[1]);

			DEBUG_REGS("after neigbours");

			// можно перейти на SSE2 only, перейдя на extract_epi16
			__kpe_left = _mm_extract_epi8(rr[0], 15);
			__kle_right = _mm_extract_epi8(rr[1], 0);

			__l_temp = _mm_extract_epi8(top[0], 15);
			__r_temp = _mm_extract_epi8(top[1], 0);
			top[0] = _mm_insert_epi8(top[0], (uint8_t) (__kle_right + __l_temp), 15);
			top[1] = _mm_insert_epi8(top[1], (uint8_t) (__kpe_left + __r_temp), 0);

			DEBUG_REGS("after insert");

			top[0] = _mm_sub_epi8(top[0], mid[0]); // готовый результат для первого регистра
			top[1] = _mm_sub_epi8(top[1], mid[1]); // готовый результат для второго регистра

			_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + offset + width), top[0]);
			DEBUG_REGS("after top0 = top0 - mid0 and STORE");

			mid[0] = _mm_setzero_si128();
			low[0] = _mm_setzero_si128();
#pragma endregion
#pragma region mid
			for (offset = (y + 1) * 32; offset < width - 16; offset += 16) {
				top[0] = top[1];
				rr[0] = rr[1];

				obj_offseted_ptr = obj_ptr + offset;

				top[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr));
				mid[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr + mid_index));
				low[1] = _mm_load_si128(reinterpret_cast<__m128i*>(obj_offseted_ptr + mid_index * 2));

				DEBUG_REGS("on first mid op");

				justSum(top[1], mid[1], low[1]);
				rr[1] = top[1];
				sumNeighbours(top[1]);
				DEBUG_REGS("after vertical sum");

				__kpe_left = _mm_extract_epi8(rr[0], 15);
				__kle_right = _mm_extract_epi8(rr[1], 0);

				__l_temp = _mm_extract_epi8(top[0], 15);
				__r_temp = _mm_extract_epi8(top[1], 0);
				top[0] = _mm_insert_epi8(top[0], (uint8_t) (__kle_right + __l_temp), 15);
				top[1] = _mm_insert_epi8(top[1], (uint8_t) (__kpe_left + __r_temp), 0);

				DEBUG_REGS("after insert");

				top[1] = _mm_sub_epi8(top[1], mid[1]); 
				_mm_store_si128(reinterpret_cast<__m128i*>(res_ptr + offset - 16 + width), top[0]);
				DEBUG_REGS("after top0 = top0 - mid0 and STORE");
#pragma endregion

				
			}
		}

		return;

		for (size_t block_id = 1; block_id < total_blocks_count - 1; block_id++) {
			
			top[0] = top[1];
			rr[0] = rr[1];


			top[1] = _mm_load_si128((__m128i*)(obj_ptr + offset));
			low[1] = _mm_loadu_si128((__m128i*)(obj_ptr + mid_index + offset)); // unaligned, т.к. смещение может быть не кратно 16

			rr[1] = _mm_add_epi8(low[1], top[1]);

			offset += 16;

			DEBUG_REGS("after block_id != 0");


			//r2[0] = _mm_slli_si128(rr[0], 1);   // сдвиг вправо на 1 байт (влево по памяти)
			//r3[0] = _mm_srli_si128(rr[0], 1);   // сдвиг влево на 1 байт (вправо по памяти)

			DEBUG_REGS("after shuffle");


			
			//r2[0] = _mm_insert_epi8(r2[0], __left, 0);
			__kpe_left = _mm_extract_epi8(rr[0], 15);

			DEBUG_REGS("after insert r2 = insert(r2)");
			

			__kle_right = _mm_extract_epi8(rr[1], 0);
			//r3[0] = _mm_insert_epi8(r3[0], __right, 15);


			//r2[0] = _mm_add_epi8(r2[0], r3[0]);		// r2 = r2 + r3

			//low[0] = _mm_add_epi8(r2[0], rr[0]);	// r1 = r2 + rr

			low[0] = _mm_sub_epi8(low[0], top[0]);	// r1 = r1 - r0

			DEBUG_REGS("after r1 = r2 + r3 - r0");

			if (block_id == total_blocks_count - 2) {
				[[unlikely]]
				// если последний блок, то сохраняем предпоследний блок
				_mm_store_si128((__m128i*)(res_ptr + 15 + (block_id - 1) * 16), low[0]);

				//r2[1] = _mm_slli_si128(rr[1], 1);   // сдвиг вправо на 1 байт (влево по памяти)
				//r3[1] = _mm_srli_si128(rr[1], 1);   // сдвиг влево на 1 байт (вправо по памяти)

				// вставляем в последний блок на нулевую позицию сумму соседей слева
				//r2[1] = _mm_insert_epi8(r2[1], __left, 0);

				//r2[1] = _mm_add_epi8(r2[1], r3[1]);
				//low[1] = _mm_add_epi8(r2[1], rr[1]);
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



	//
	//		auto firstHorizontallSum = [&left, &right, &justSum, &_DEBUG_REG, &sumNeighbours] (__m128i r00, __m128i r01, __m128i r10, __m128i r11, __m128i r20, __m128i r21) -> array<__m128i, 2> {
	//
	//		};
	//
	//		auto horizontallSum = [&left, &right, &justSum, &_DEBUG_REG, &sumNeighbours] (__m128i r00, __m128i r01, __m128i r10, __m128i r11, __m128i r20, __m128i r21) -> array<__m128i, 2> {
	//			auto res = array<__m128i, 2> {r10, r11};
	//
	//			__m128i saver00 = r00, saver01 = r01;
	//
	//			justSum(r00, r10, r20); // r00 = r00 + r10 + r20
	//			justSum(r01, r11, r21); // r01 = r01 + r11 + r21
	//			
	//			right = _mm_extract_epi8(r01, 0);
	//
	//			_DEBUG_REG(r00, "r00 vertical sum");
	//			_DEBUG_REG(r01, "r01 vertical sum");
	//
	//			sumNeighbours(r00);
	//			sumNeighbours(r01);
	//
	//			left = _mm_extract_epi8(r00, 15);
	//
	//			_DEBUG_REG(r00, "r00 neighbour sum");
	//			_DEBUG_REG(r01, "r01 neighbour sum");
	//
	//			r00 = _mm_insert_epi8(r00, (uint8_t) (left + right), 15);
	//
	//			res[0] = _mm_sub_epi8(r00, res[0]);
	//
	//			return res;
	//		};
	//
	//		size_t TOTAL_FULLY_IN_BLOCKS_COUNT = width / 14;
	//		size_t REMAINDER = width % 14;
	//
	//#pragma region top
	//		// верхние ряды
	//
	//		for (size_t x = 0; x < width; x += 32) {
	//			__m128i r01, r11, r21;
	//
	//			r0  = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + x));
	//			r01 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + x + 16));
	//
	//			r1  = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width + x));
	//			r11 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width + x + 16));
	//
	//			r2  = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width * 2 + x));
	//			r21 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width * 2 + x + 16));
	//
	//			horizontallSum(r0, r01, r1, r11, r2, r21);
	//			DEBUG_RES("after second row");
	//		}
	//#pragma endregion

	//
	//		
	//
	//		for (size_t i = 2; i < height - 1; i++) {
	//			const size_t offset = width * i;
	//
	//			r0 = r1;
	//			r1 = r2;
	//			r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset + width));
	//
	//			calc_three_lines(r0, r1, r2, offset);
	//		}
	//		calc_for_two_lines(r2, r1, width * (height - 1));
	//#pragma endregion
	//
	//#pragma region mid
	//		// ряды по середине
	//		for (size_t offset_from_zero = 1, i = 0; i < TOTAL_FULLY_IN_BLOCKS_COUNT; offset_from_zero += 14, i++) {
	//
	//			r0 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset_from_zero));
	//			r1 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset_from_zero + width));
	//			r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset_from_zero + 2 * width));
	//
	//			calc_for_two_lines_with_offset(r0, r1, offset_from_zero);
	//			//DEBUG_RES("after first row");
	//
	//			calc_three_lines_with_offset(r0, r1, r2, offset_from_zero + width);
	//			//DEBUG_RES("after second row");
	//
	//			for (size_t i = 2; i < height - 1; i++) {
	//				const size_t offset = offset_from_zero + width * i;
	//
	//				r0 = r1;
	//				r1 = r2;
	//				r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset + width));
	//
	//				calc_three_lines_with_offset(r0, r1, r2, offset);
	//			}
	//			calc_for_two_lines_with_offset(r2, r1, offset_from_zero + width * (height - 1));
	//			//DEBUG_RES("after last row");
	//		}
	//		DEBUG_RES("after main");
	//#pragma endregion
	//
	//#pragma region last
	//		if (REMAINDER == 0) return;
	//
	//		// блок, который не вошёл полностью, то есть крайние правые строки
	//		r0 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width - 16));
	//		r1 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + 2 * width - 16));
	//		r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + 3 * width - 16));
	//
	//		calc_two_last_lines(r0, r1, width - 16);
	//
	//		calc_three_last_lines(r0, r1, r2, 2 * width - 16);
	//		DEBUG_RES("NEED THIS");
	//
	//		for (size_t i = 2; i < height - 1; i++) {
	//			const size_t offset = width * i - 16;
	//
	//			r0 = r1;
	//			r1 = r2;
	//			r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset + width));
	//
	//			calc_three_last_lines(r0, r1, r2, offset);
	//		}
	//		calc_two_last_lines(r2, r1, height * width - 16);
	//
	//		DEBUG_RES("after all main");
	//#pragma endregion
		}
	}
};
