#include "CG_flat_sse.hpp"
#include <emmintrin.h>


using namespace Benchmark;

using std::thread;



CaveGenerator_flat_sse::CaveGenerator_flat_sse(size_t width, size_t height, bool randInit) noexcept:
	CaveGeneratorBench(width, height, randInit)
{
	InitMatrix(randInit);
	name = "Flat<byte> sse";
}

CaveGenerator_flat_sse::CaveGenerator_flat_sse(size_t width, size_t height, int threadsCount, bool randInit) noexcept:
	CaveGeneratorBench(width, height, threadsCount, randInit)
{
	InitMatrix(randInit);
	InitThreads(threadsCount);
	name = "Flat<byte> sse";
}

CaveGenerator_flat_sse::CaveGenerator_flat_sse(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept:
	CaveGeneratorBench(width, height, threadsCount)
{
	InitMatrix(false);
	InitThreads(threadsCount);

	const size_t capacity = width * height;

	for (size_t x = 0; x < capacity; x++) {
		_mainMatrix->at(x) = vec[x];
	}

	name = "Flat<byte> sse";
}


void CaveGenerator_flat_sse::InitMatrix(bool randInit) noexcept
{
	_mainMatrix = new Flat2DArray<byte>(_width, _height);
	_secondMatrix = new Flat2DArray<byte>(_width, _height);

	if (randInit) {
		FillArrayRandomBool<byte>(_width * _height, _mainMatrix->data());
	}
}

void CaveGenerator_flat_sse::InitThreads(int threadsCount) noexcept
{
	const size_t CHUNK_SIZE = (this->_height + _threadsCount - 1) / _threadsCount;

	_CHUNKS_INDEXES.reserve(size_t(_threadsCount + 1));

	const size_t mainMatrixWidth = _mainMatrix->width();

	for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
		size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

		if (LineFrom + CHUNK_SIZE > mainMatrixWidth && LineFrom != mainMatrixWidth) {
			LineTo = mainMatrixWidth;
		}

		if (_CHUNKS_INDEXES.size() == 0)
			_CHUNKS_INDEXES.push_back((int)LineFrom);

		_CHUNKS_INDEXES.push_back((int)LineTo);
	}
}

inline static void runVerticalSum(Flat2DArray<byte>* object, Flat2DArray<byte>* to_save) noexcept {
	const size_t width = object->width();
	const size_t height = object->height();

	byte* res_ptr = to_save->data();
	byte* data_ptr = object->data();

	__m128i r0,		// хранит верхнюю строку окна
		r1,			// хранит среднюю строку окна
		r2;			// хранит нижнюю строку окна
#define _DEBUG_SSE_VERTICAL 0
#if defined(_DEBUG) && _DEBUG_SSE_VERTICAL == 1

	auto _DEBUG_REG = [] (__m128i reg, string msg) {
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
	auto sum_three_line = [](__m128i r0, __m128i r1, __m128i r2) -> __m128i {
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
		byte prev_res = res_ptr[offset + 16];
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
		// выравнивание гарантированно, т.к. Flat2DArray выровнен и оффсет кратен 2
		_mm_store_si128((__m128i*)(res_ptr + offset + 1), sum);
	};


	auto calc_two_last_lines = [&res_ptr, &sum_two_lines] (__m128i r0, __m128i r1, size_t offset) {
		__m128i sum = sum_two_lines(r0, r1);

		byte prev_res = res_ptr[offset + 16];
		sum = _mm_srli_si128(sum, 1); // сдвигаю результат влево на 1 байт
		_mm_store_si128((__m128i*)(res_ptr + offset + 1), sum);
		res_ptr[offset + 16] = prev_res;
	};

	size_t TOTAL_FULLY_IN_BLOCKS_COUNT = width / 14;
	size_t REMAINDER = width % 16;

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
	for (size_t offset_from_zero = 1, i = 1; i < TOTAL_FULLY_IN_BLOCKS_COUNT; offset_from_zero += 14, i++) {

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
	if (REMAINDER == 0) return;

	// блок, который не вошёл полностью, то есть крайние правые строки
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

	DEBUG_RES("after all main");
#pragma endregion
}

inline int CaveGenerator_flat_sse::GetNeighbours(size_t x, size_t y) const noexcept {
	return 42;
}

void CaveGenerator_flat_sse::Tick(const int count) noexcept
{
	auto applyRules = [&] () {
		const size_t capacity = this->_width * this->_height;
		byte* dataPtr = _secondMatrix->data();
		byte* neighboursPtr = _secondMatrix->data();
		for (size_t x = 0; x < capacity; x++) {
			byte& at = dataPtr[x];
			at = (at && this->S.contains(neighboursPtr[x])) ||
				(!at && this->B.contains(neighboursPtr[x]));
		}
		return;
	};

	for (int i = 0; i < count; i++) {
		runVerticalSum(this->_mainMatrix, this->_secondMatrix);
		applyRules();

		std::swap(_mainMatrix, _secondMatrix);
	}
}

void CaveGenerator_flat_sse::TickMT(const int count) noexcept
{
	static const size_t THREADS_COUNT = this->_threadsCount;
	static const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

	vector<thread> THREADS;


	for (int i = 0; i < count; i++) {
		THREADS.reserve(THREADS_COUNT);

		for (size_t idx = 0; idx < this->_CHUNKS_INDEXES.size() - 1; idx++) {  
			// пары идут в порядке [x, x+1] , [x+1, x+2], [x+2, x+3]
			if (idx + 1 < _CHUNKS_INDEXES.size()) {
				size_t start = (size_t)_CHUNKS_INDEXES[idx];
				size_t end = (size_t)_CHUNKS_INDEXES[idx + 1];

				THREADS.emplace_back([this, start, end]() {
					this->TickMTRealization(start, end);
					});
			}
		}

		

		for (auto& th : THREADS) {
			th.join();
		}
		THREADS.clear();

		// Меняем местами матрицы
		std::swap(_mainMatrix, _secondMatrix);
	}
}

void CaveGenerator_flat_sse::TickMTRealization(const size_t lineFrom, const size_t lineTo) noexcept
{
	size_t xFrom = lineFrom;

	int neighbours = 0;

	auto applyRules = [&](size_t x, size_t y, int neighbours) {
		byte& at = this->_secondMatrix->at(x, y);
		at = (at && this->S.contains(neighbours)) ||
			(!at && this->B.contains(neighbours));
		return;
	};
}

bool* CaveGenerator_flat_sse::Data() noexcept
{
	return (bool*)(_mainMatrix->data());
}

CaveGenerator_flat_sse::~CaveGenerator_flat_sse() noexcept {
	delete _mainMatrix;
	delete _secondMatrix;
	_width = 0;
	_height = 0;
	_threadsCount = 0;
}

ostream& operator<<(ostream& stream, const CaveGenerator_flat_sse& cave) noexcept
{
	for (size_t y = 0; y < cave._height; y++) {
		for (size_t x = 0; x < cave._width; x++) {
			stream << cave._mainMatrix->at(x, y) << ' ';
		}
		stream << "\n";
	}

	return stream;
}
