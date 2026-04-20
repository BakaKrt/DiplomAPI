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

	#define print_two_m128i_uint(arr, name) ((void)0)
#endif // _DEBUG


	


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
		
		constexpr size_t window_size = 2;

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
	Flat2DArray<T> run_horizontalSumImpl(Flat2DArray<T>& object) const noexcept {
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

		//size_t* indexes = new size_t[height * 2];
		auto indexes = Flat2DArray<int>(height, 1, false);

		for (size_t x = 0; x < height; x++) {
			indexes[x] = (x + 1) * width - 2;
		}
		//indexes[height - 1] = height * width - 4;

		__m128i a = _mm_loadl_epi64((__m128i*)(data_ptr + width * height - 2));
		for (size_t x = 0; x < height; x++) {

		}


		return object;
	}
};
