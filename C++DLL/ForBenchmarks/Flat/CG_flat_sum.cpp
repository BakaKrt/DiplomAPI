#include "CG_flat_sum.hpp"


using namespace Benchmark;

using std::thread;



CaveGenerator_flat_sum::CaveGenerator_flat_sum(size_t width, size_t height, bool randInit) noexcept:
	CaveGeneratorBench(width, height, randInit)
{
	InitMatrix(randInit);
	name = "Flat<bool> sum";
}

CaveGenerator_flat_sum::CaveGenerator_flat_sum(size_t width, size_t height, int threadsCount, bool randInit) noexcept:
	CaveGeneratorBench(width, height, threadsCount, randInit)
{
	InitMatrix(randInit);
	InitThreads(threadsCount);
	name = "Flat<bool> sum";
}

CaveGenerator_flat_sum::CaveGenerator_flat_sum(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept:
	CaveGeneratorBench(width, height, threadsCount)
{
	InitMatrix(false);
	InitThreads(threadsCount);

	const size_t capacity = width * height;

	for (size_t x = 0; x < capacity; x++) {
		_mainMatrix->at(x) = vec[x];
	}

	name = "Flat<bool> sum";
}


void CaveGenerator_flat_sum::InitMatrix(bool randInit) noexcept
{
	_mainMatrix = new Flat2DArray<bool>(_width, _height);
	_secondMatrix = new Flat2DArray<bool>(_width, _height);

	if (randInit) {
		FillArrayRandomBool<bool>(_width * _height, _mainMatrix->data());
	}
}

void CaveGenerator_flat_sum::InitThreads(int threadsCount) noexcept
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

/// <summary>
/// Считает количество соседей для клетки с координатами x и y в _mainMatrix
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
int CaveGenerator_flat_sum::GetNeighbours(size_t x, size_t y) const noexcept
{
	const size_t offset_to_top_left = this->_width * (y - 1) + x - 1;
	bool* ptr = _mainMatrix->data() + offset_to_top_left;

	return ptr[0] + ptr[1] + ptr[2] + \
		ptr[_width] + ptr[_width + 2] + \
		ptr[_width * 2] + ptr[_width * 2 + 1] + ptr[_width * 2 + 2];
}

void CaveGenerator_flat_sum::Tick(const int count) noexcept
{
#ifdef _DEBUG
	auto DEBUG_RES = [&](string at_moment) {
		std::cout << "second_matrix" << at_moment << "\n";
		_secondMatrix->_debug_print_as_arrays(16);
	};
#else
	#define DEBUG_RES(at_moment) ((void)0)
#endif // DEBUG


	bool* ptr = _mainMatrix->data();
	bool* secondPtr = _secondMatrix->data();
	const size_t width = this->_width;
	const size_t height = this->_height;
	const size_t capacity = width * height;

	auto GNTop = [&]() noexcept {
		for (size_t i = 1; i < width - 1; i++) {
			int neighbours = ptr[i - 1] + ptr[i + 1] + ptr[i - 1 + width] + ptr[i + width] + ptr[i + 1 + width];
			secondPtr[i] = (ptr[i] == 0) ? B.contains(neighbours) : S.contains(neighbours);
		}
	};
	auto GNBot = [&]() noexcept {
		for (size_t i = capacity - width + 1; i < capacity - 1; i++) {
			int neighbours = ptr[i - 1 - width] + ptr[i - width] + ptr[i + 1 - width] + ptr[i - 1] + ptr[i + 1];
			secondPtr[i] = (ptr[i] == 0) ? B.contains(neighbours) : S.contains(neighbours);
		}
	};

	auto GNLeft = [&]() noexcept {
		for (size_t y = 1; y < height - 1; y++) {
			int neighbours = _mainMatrix->at(0, y - 1) +  _mainMatrix->at(1, y - 1) +  _mainMatrix->at(1, y) +  _mainMatrix->at(0, y + 1) + _mainMatrix->at(1, y + 1);
			_secondMatrix->at(0, y) = _mainMatrix->at(0, y) == 0 ? B.contains(neighbours) : S.contains(neighbours);
		}
	};
	auto GNRight = [&]() noexcept {
		const size_t x = width - 1;
		for (size_t y = 1; y < height - 1; y++) {
			int neighbours = _mainMatrix->at(x - 1, y - 1) + _mainMatrix->at(x, y - 1) +  _mainMatrix->at(x - 1, y) +  _mainMatrix->at(x - 1, y + 1) + _mainMatrix->at(x, y + 1);
			_secondMatrix->at(x, y) = _mainMatrix->at(x, y) == 0 ? B.contains(neighbours) : S.contains(neighbours);
		}
	};

	auto GNTopLeft = [&ptr, width]() noexcept -> int{ return ptr[1] + ptr[width] + ptr[width + 1];};
	auto GNTopRight = [&ptr, width] () noexcept -> int{ return ptr[width - 2] + ptr[2 * width - 2] + ptr[2 * width - 1];};
	auto GNBotLeft = [&ptr, width, capacity]() noexcept -> int{	return ptr[capacity - 2 * width] + ptr[capacity - 2 * width + 1] + ptr[capacity - width];};
	auto GNBotRight = [&ptr, width, capacity] () noexcept -> int{ return ptr[capacity - width - 2] + ptr[capacity - width - 1] + ptr[capacity - 2];};

	auto applyRules = [&] (int neighboursCount, size_t x, size_t y) {
		bool& secondAt = this->_secondMatrix->at(x, y);
		bool& mainAt = this->_mainMatrix->at(x, y);
		secondAt = (mainAt == 0) ? B.contains(neighboursCount) : S.contains(neighboursCount);
	};

	for (int i = 0; i < count; i++) {
		
		applyRules(GNTopLeft(), 0, 0);			// верхний левый угол
		GNTop();								// вверх
		applyRules(GNTopRight(), width - 1, 0);	// верхний правый угол

		GNLeft();
		for (size_t y = 1; y < _height - 1; y++) {		
			for (size_t x = 1; x < _width - 1; x++) {
				int neighbours = GetNeighbours(x, y);

				applyRules(neighbours, x, y);
			}
		}
		GNRight();

		applyRules(GNBotLeft(), 0, height - 1);
		GNBot();
		applyRules(GNBotRight(), width - 1, height - 1);


		std::swap(_mainMatrix, _secondMatrix);
	}
}

void CaveGenerator_flat_sum::TickMT(const int count) noexcept
{
	// NOT IMPLEMENTED
	return;

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

void CaveGenerator_flat_sum::TickMTRealization(const size_t lineFrom, const size_t lineTo) noexcept
{
	size_t x = lineFrom;
	for (; x < lineTo; x++) {
		for (size_t y = 0; y < this->_width; y++) {
			int neighbours = GetNeighbours(x, y);
			bool& at = this->_secondMatrix->at(x, y);
			at = (at && this->S.count(neighbours)) ||
				(!at && this->B.count(neighbours));
		}
	}
}

bool* CaveGenerator_flat_sum::Data() noexcept
{
	return _mainMatrix->data();
}


CaveGenerator_flat_sum::~CaveGenerator_flat_sum() noexcept {
	delete _mainMatrix;
	delete _secondMatrix;
	_width = 0;
	_height = 0;
	_threadsCount = 0;
}

ostream& operator<<(ostream& stream, const CaveGenerator_flat_sum& cave) noexcept
{
	for (size_t y = 0; y < cave._height; y++) {
		for (size_t x = 0; x < cave._width; x++) {
			stream << cave._mainMatrix[x][y] << ' ';
		}
		stream << "\n";
	}

	return stream;
}
