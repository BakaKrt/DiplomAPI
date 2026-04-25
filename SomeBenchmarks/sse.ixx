#include <smmintrin.h> // SSE4.1

export module sse;

import std;
import test;

using std::string;
using std::array;

using std::memcpy;
using std::printf;


export class SSEv1Sum : public test<SSEv1Sum> {
public:
	SSEv1Sum() { name = "sse v1"; }

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	inline void test_runImpl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		run_verticalSumAll(object, to_save);

	}

#ifdef _DEBUG
	static void print_m128_float(const __m128& reg, std::string name) {
		alignas(__m128) float buf[4];
		_mm_store_ps(buf, reg);
		std::printf("%s:", name.c_str());
		for (int i = 0; i < 4; ++i) std::printf(" %f", buf[i]);
	};
	static void print_m128i_uint8(__m128i reg, std::string name) {
		alignas(__m128i)  uint8_t buf[16] {};
		_mm_store_si128((__m128i*)buf, reg);
		std::printf("%5s:", name.c_str());
		for (int i = 0; i < 16; ++i) std::printf("%3u ", (uint8_t)buf[i]);
	};

	static void print_m128i_uint8_half(__m128i reg, std::string name) {
		alignas(__m128i)  uint8_t buf[16] {};
		_mm_store_si128((__m128i*)buf, reg);
		std::printf("%5s:", name.c_str());
		for (int i = 0; i < 8; ++i) std::printf("%3u ", (uint8_t) buf[i]);
	};

    static void print_two_m128i_uint(const array<__m128i, 2>& arr, const std::string& name) {
		for (size_t i = 0; i < arr.size(); i++) {
			// вывод в консоль в формате: name<i>: values
			print_m128i_uint8(arr[i], name + std::to_string(i));
		}
		std::printf("\n");
	}

#else
	#define print_m128_float(reg, name) ((void)0)
	#define print_m128_uint8(reg, name) ((void)0)
	#define print_m128_uint8_half(reg, name) ((void)0)

	#define print_two_m128i_uint(arr, name) ((void)0)
#endif // _DEBUG
	

	template<typename T> requires allowed_type<T>
	inline Flat2DArray<T> run_horizontalSumImpl(Flat2DArray<T>& object) const noexcept {
		// ширина - это количество элементов в строке
		const size_t width = object.width();

		// результатом будет массив 2 * (width - 2) - так как крайние элементы не имеют двух соседей
		auto res = Flat2DArray<T>(width - 2, 1, false);

		auto res_ptr = res.data();
		auto obj_ptr = object.data();

		// индекс начала нижней строки (то есть 1-ой строки) [0-ая это верхняя]
		const size_t mid_index = width;

		// количество блоков, которые полностью помещаются в ширину
		const size_t blocks_count = mid_index / 16;

		// количество блоков, которые не полностью помещаются в ширину. Если ширина не кратна 16, то последний блок будет обрабатываться отдельно
		const size_t remainder = width % 16;

		constexpr size_t window_size = 2;

		array<__m128i, window_size> top {};
		array<__m128i, window_size> low {};
		array<__m128i, window_size> rr {};	// сохраняю сумму top + low

		array<__m128i, window_size> r2 {};
		array<__m128i, window_size> r3 {};

		// для отладки. В релизе эти функции не будут вызываться, так как макрос DEBUG_REGS() будет пустой функцией
#ifdef _DEBUG
		auto DEBUG_REGS = [&] (string at_moment) {
			std::printf("regs %s\n", at_moment.c_str());
			print_two_m128i_uint(top, "top");
			print_two_m128i_uint(low, "low");
			print_two_m128i_uint(rr, "rr");
			print_two_m128i_uint(r2, "r2");
			print_two_m128i_uint(r3, "r3");
			print_two_m128i_uint(low, "low");
			std::printf("\n");
		};

#else
#define DEBUG_REGS(at_moment) ((void)0)
#endif // _DEBUG

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
		return res;
	}

	template<typename T> requires allowed_type<T>
	inline Flat2DArray<T> run_verticalSumImpl(Flat2DArray<T>& object) const noexcept {
		const size_t width = object.width();
		const size_t height = object.height();

		const size_t offset_from_zero = 0;

		auto res = Flat2DArray<T>(14, height - 2, false);

		T* res_ptr = res.data();
		T* data_ptr = object.data();

		__m128i r0, r1, r2, saver1 {}, saver2 {};

		size_t* indexes = new size_t[height];

		for (short i = 0; i < height; i++) {
			indexes[i] = offset_from_zero + i * width;
		}

		// для отладки. В релизе эти функции не будут вызываться, так как макрос DEBUG_REGS() будет пустой функцией
#ifdef _DEBUG
		auto DEBUG_REGS = [&] (string at_moment) {
			std::printf("regs %s\n", at_moment.c_str());
			print_m128i_uint8(r0, "r0");
			print_m128i_uint8(r1, "r1"); std::printf("\n");
			print_m128i_uint8(r2, "r2");
			print_m128i_uint8(saver1, "rr"); std::printf("\n");
		};
#else
	#define DEBUG_REGS(at_moment) ((void)0)
#endif // _DEBUG

		auto run = [&] (size_t offset) { 
			saver1 = r1;					// rmid = r1 значения по середине окна сохраняю в saver1, так как они нужны для вычитания в конце
			saver2 = r2;

			DEBUG_REGS("after load");

			r1 = _mm_add_epi8(r1, r2);
			r0 = _mm_add_epi8(r0, r1);		// r0 = r2 + r1 + r0

			r1 = _mm_srli_si128(r0, 1);		// сдвиг влево на 1 байт (влево по памяти)
			r2 = _mm_slli_si128(r0, 1);		// сдвиг вправо на 1 байт (влево по памяти)

			DEBUG_REGS("after shift");

			r1 = _mm_add_epi8(r1, r2);
			r0 = _mm_add_epi8(r0, r1);		// r0 = r2 + r1 + r0
			DEBUG_REGS("r0 = r2 + r1 + r0");

			r0 = _mm_sub_epi8(r0, saver1);	// r0 = r0 - rres
			DEBUG_REGS("r0 = r0 - rr");

			r0 = _mm_srli_si128(r0, 1);		// r0 = mask1(r0)
			DEBUG_REGS("on save");
			_mm_storeu_si128((__m128i*)(res_ptr + offset), r0);
		};

		r0 = _mm_load_si128((__m128i*)(data_ptr + indexes[0])); 
		r1 = _mm_load_si128((__m128i*)(data_ptr + indexes[1]));
		r2 = _mm_load_si128((__m128i*)(data_ptr + indexes[2]));
		run(0);
		
		for (size_t i = 1; i < height - 2; i++) {
			r0 = saver1;
			r1 = saver2;
			r2 = _mm_load_si128((__m128i*)(data_ptr + indexes[i + 2]));

			const size_t offset = i * 14;
			run(offset);
		}

		delete[] indexes;

		return res;
	}

	template<typename T> requires allowed_type<T>
	Flat2DArray<T> run_horizontalNextLineSum(Flat2DArray<T>& object) const noexcept {
		const size_t width = object.width();
		const size_t height = object.height();

		auto res = Flat2DArray<T>(height * 2, 1, false);

		T* data_ptr = object.data();
		T* res_ptr = res.data();

#ifdef _DEBUG
		auto indexes = Flat2DArray<int>(height, 1, false);
#else
		size_t* indexes = new size_t[height];
#endif // _DEBUG
				

		for (size_t x = 0; x < height; x++) {
			indexes[x] = (x + 1) * width - 2;
		}

		__m128i r0, r1, r2, s1, s2;

		T *row_0 = nullptr,
		  *row_1 = nullptr,
		  *row_2 = nullptr;


#ifdef _DEBUG
		auto DEBUG_REGS = [&] (string at_moment) {
			std::printf("regs %s\n", at_moment.c_str());
			print_m128i_uint8_half(r0, "r0");
			print_m128i_uint8_half(r1, "r1"); std::printf("\n");
			print_m128i_uint8_half(r2, "r2"); std::printf("\n");
			print_m128i_uint8_half(s1, "s1");
			print_m128i_uint8_half(s2, "s2"); std::printf("\n");
			};
#else
#define DEBUG_REGS(at_moment) ((void)0)
#endif // _DEBUG

#pragma region first
		// не имеет смысла использовать SIMD для первых трёх элементов, так как всего 3 соседа дают очень маленькую эффективность
		row_0 = data_ptr;
		row_1 = data_ptr + indexes[0];
		row_2 = data_ptr + indexes[1];

		T first = row_0[1] + row_1[2] + row_1[3]; res[0] = first;
		T second = row_1[0] + row_2[0] + row_2[1]; res[1] = second;
		T third = row_0[0] + row_0[1] + row_1[3] + row_2[2] + row_2[3]; res[2] = third;

		r0 = _mm_loadl_epi64((__m128i*)row_0);
		r1 = _mm_loadl_epi64((__m128i*)row_1);
		r2 = _mm_loadl_epi64((__m128i*)row_2);
#pragma endregion
#pragma region mid
		const __m128i mask0 = _mm_setr_epi8(0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3);
		const __m128i mask1 = _mm_setr_epi8(0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2);

		for (size_t x = 0, save_at = 3; x < height - 2; x++, save_at += 2) {
			row_2 = data_ptr + indexes[x + 2];

			r0 = r1;
			r1 = r2;
			r2 = _mm_loadl_epi64((__m128i*)row_2);

			s1 = _mm_add_epi8(r0, r2);			// s1 = r1 + r2
			s2 = _mm_srli_si128(s1, 1);			// s2 = s1 << 1

			DEBUG_REGS("on load");

			s1 = _mm_add_epi8(s1, s2);			// s1 = s1 + s2
			s2 = _mm_shuffle_epi8(r1, mask0);	// s2 = mask0(r1) 

			DEBUG_REGS("after s1 = s1 + s2 and shuffle s2 = mask0(r1)");

			s1 = _mm_add_epi8(s1, s2);			// s1 = s1 + s2
			s1 = _mm_shuffle_epi8(s1, mask1);	// s1 = mask1(s1)
			 
			DEBUG_REGS("before save");

			s1 = _mm_srli_si128(s1, 14);
			DEBUG_REGS("after shift");

			_mm_storel_epi64((__m128i*)(res_ptr + save_at), s1);
		}
#pragma endregion
#pragma region last
		row_1 = data_ptr + indexes[height - 2];
		T sum = row_1[0] + row_1[1] + row_2[0];
		res[height * 2 - 1] = sum;
#pragma endregion

#ifdef NDEBUG
		delete[] indexes;
#endif // _NDEBUG


		return res;
	}

	/// <summary>
	/// Не самая быстрая реализация, так как идёт итерация по вертикали, а не по горизонтали
	/// Из-за этого скорее всего страдает кеш-локальность данных, что не даёт получить ещё большую скорость выполнения
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="object"></param>
	/// <param name="to_save"></param>
	template<typename T> requires allowed_type<T>
	inline void run_verticalSumAll(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		const size_t width = object.width();
		const size_t height = object.height();

		T* res_ptr = to_save.data();
		T* data_ptr = object.data();

		__m128i r0,		// хранит верхнюю строку окна
			r1,			// хранит среднюю строку окна
			r2;			// хранит нижнюю строку окна

#ifdef _DEBUG

		auto _DEBUG_REG = [] ( __m128i reg, string msg) {
			std::printf("reg: %s\n", msg.c_str());
			print_m128i_uint8(reg, "reg"); printf("\n");
		};

		auto DEBUG_RES = [&to_save] (string at_moment) {
			std::cout << "to_save " << at_moment << "\n" << to_save << "\n";
		};
#else
		#define _DEBUG_REG(reg, at_moment) ((void)0)
		#define DEBUG_RES(at_moment) ((void)0)
#endif // _DEBUG
		
		// суммирует 3 строки вида:
		// r0 =   0   1   2   3   4   5 ...
		// r1 =  15  16  17  18  19  20 ...
		// r2 =  30  31  32  33  34  35 ...
		// res[0] = 0 + 1 + 16+ 30 + 31
		// res[1] = 0 + 1 + 2 + 15 + 17 + 30 + 31 + 32
		// res[2] = 1 + 2 + 3 + 16 + 18 + 31 + 32 + 33
		// и ь.д
		auto sum_three_line = [] (__m128i r0, __m128i r1, __m128i r2) -> __m128i {
			__m128i mid_row = r1;

			// в r2 будет хранится вся сумма
			r2 = _mm_add_epi8(r1, r2);
			r2 = _mm_add_epi8(r0, r2);		// r2 = r0 + r1 + r2

			r1 = _mm_slli_si128(r2, 1);		// сдвиг вправо на 1 байт (сосед справа)
			r0 = _mm_srli_si128(r2, 1);		// сдвиг влево на 1 байт (сосед слева)

			r2 = _mm_add_epi8(r2, r1);		// r1 = сам + сосед справа
			r2 = _mm_add_epi8(r2, r0);		// r1 = сам + сосед справа + сосед слева = 9 клеток

			r2 = _mm_sub_epi8(r2, mid_row);	// r1 = сумма всех 8 соседних клеток = 9 клеток - центральная клетка
			return r2;
		};

		// когда просто сохраняем сумму соседей
		auto calc_three_lines = [&res_ptr, &sum_three_line] (__m128i r0, __m128i r1, __m128i r2, size_t offset) {
			__m128i sum = sum_three_line(r0, r1, r2);
			_mm_storeu_si128((__m128i*)(res_ptr + offset), sum);
		};
		
		// для случаев, когда нужно сохранить с перекрытием предыдущего ответа
		auto calc_three_lines_with_offset = [&res_ptr, &sum_three_line] (__m128i r0, __m128i r1, __m128i r2, size_t offset) {
			__m128i sum = sum_three_line(r0, r1, r2);
			sum = _mm_srli_si128(sum, 1); // сдвигаю результат влево на 1 байт
			_mm_storeu_si128((__m128i*)(res_ptr + offset + 1), sum);
		};

		// для крайних правых строк
		auto calc_three_last_lines = [&res_ptr, &sum_three_line] (__m128i r0, __m128i r1, __m128i r2, size_t offset) {
			__m128i sum = sum_three_line(r0, r1, r2);

			// приходится сохранять значение следующее после offset, так как оно затирается после сдвига и сохранения на +1
			T prev_res = res_ptr[offset + 16];
			sum = _mm_srli_si128(sum, 1); // сдвигаю результат влево на 1 байт
			_mm_storeu_si128((__m128i*)(res_ptr + offset + 1), sum);
			res_ptr[offset + 16] = prev_res;
		};


		// суммирует 2 строки вида:
		// r0 =   0   1   2   3   4   5 ...
		// r1 =  15  16  17  18  19  20 ...
		// res[0] = 1 + 15 + 16
		// res[1] = 0 + 2 + 15 + 16 + 17
		// res[2] = 1 + 3 + 16 + 17 + 18 + 31 + 32 + 33
		// и т.д.
		auto sum_two_lines = [](__m128i r0, __m128i r1) -> __m128i {
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

		// вычисляет сумму для двух строчек. В зависимости от порядка регистров,
		// можно вычислить как для верхней строчки, так и для последней
		auto calc_for_two_lines = [&res_ptr, &sum_two_lines] (__m128i r0, __m128i r1, size_t offset) {
			__m128i sum = sum_two_lines(r0, r1);
			_mm_storeu_si128((__m128i*)(res_ptr + offset), sum);
		};

		auto calc_for_two_lines_with_offset = [&res_ptr, &sum_two_lines] (__m128i r0, __m128i r1, size_t offset) {
			__m128i sum = sum_two_lines(r0, r1);
			sum = _mm_srli_si128(sum, 1);	// сдвигаю результат влево на 1 байт,
			_mm_storeu_si128((__m128i*)(res_ptr + offset + 1), sum);
		};


		auto calc_two_last_lines = [&res_ptr, &sum_two_lines] (__m128i r0, __m128i r1, size_t offset) {
			__m128i sum = sum_two_lines(r0, r1);

			T prev_res = res_ptr[offset + 16];
			sum = _mm_srli_si128(sum, 1); // сдвигаю результат влево на 1 байт
			_mm_storeu_si128((__m128i*)(res_ptr + offset + 1), sum);
			res_ptr[offset + 16] = prev_res;
		};
		
		size_t TOTAL_FULLY_IN_BLOCKS_COUNT = width / 14;
		size_t REMAINDER = width % 14;
		
#pragma region left
		// крайние левые ряды
		r0 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr));
		r1 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width));
		r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width * 2));

		calc_for_two_lines(r0, r1, 0);
		//DEBUG_RES("after first row");

		calc_three_lines(r0, r1, r2, width);
		//DEBUG_RES("after second row");

		for (size_t i = 2; i < height - 1; i++) {
			const size_t offset = width * i;

			r0 = r1;
			r1 = r2;
			r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset + width));

			calc_three_lines(r0, r1, r2, offset);
		}
		calc_for_two_lines(r2, r1, width * (height - 1));
#pragma endregion

#pragma region mid
		// ряды по середине
		for (size_t offset_from_zero = 1, i = 0; i < TOTAL_FULLY_IN_BLOCKS_COUNT; offset_from_zero += 14, i++) {

			r0 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset_from_zero));
			r1 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset_from_zero + width));
			r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset_from_zero + 2 * width));

			calc_for_two_lines_with_offset(r0, r1, offset_from_zero);
			//DEBUG_RES("after first row");

			calc_three_lines_with_offset(r0, r1, r2, offset_from_zero + width);
			//DEBUG_RES("after second row");

			for (size_t i = 2; i < height - 1; i++) {
				const size_t offset = offset_from_zero + width * i;

				r0 = r1;
				r1 = r2;
				r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset + width));

				calc_three_lines_with_offset(r0, r1, r2, offset);
			}
			calc_for_two_lines_with_offset(r2, r1, offset_from_zero + width * (height - 1));
			//DEBUG_RES("after last row");
		}
		DEBUG_RES("after main");
#pragma endregion
				
#pragma region last
		// блок, который не вошёл полностью, то есть крайние правые строки
		if (REMAINDER > 0) {

			r0 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + width - 16));
			r1 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + 2 * width - 16));
			r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + 3 * width - 16));

			calc_two_last_lines(r0, r1, width - 16);

			calc_three_last_lines(r0, r1, r2, 2 * width - 16);
			DEBUG_RES("NEED THIS");

			for (size_t i = 2; i < height - 1; i++) {
				const size_t offset = width * i - 16;

				r0 = r1;
				r1 = r2;
				r2 = _mm_load_si128(reinterpret_cast<__m128i*>(data_ptr + offset + width));

				calc_three_last_lines(r0, r1, r2, offset);
			}
			calc_two_last_lines(r2, r1, height * width - 16);
		}
		DEBUG_RES("after all main");
#pragma endregion
	}
};
