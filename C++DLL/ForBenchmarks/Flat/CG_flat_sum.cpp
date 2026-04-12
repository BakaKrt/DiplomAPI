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
	if (x > 0 && x + 1 < _width && y > 0 && y + 1 < _height) {
		const array<int, 8> objects{
			_mainMatrix->at(x - 1,y - 1), // вл
			_mainMatrix->at(x - 1,y	   ), // л
			_mainMatrix->at(x - 1,y + 1), // нл 

			_mainMatrix->at(x	 ,y - 1), // вв
			_mainMatrix->at(x	 ,y + 1), // нн

			_mainMatrix->at(x + 1,y - 1), // вп
			_mainMatrix->at(x + 1,y	   ), // п
			_mainMatrix->at(x + 1,y + 1)  // нп
		};

		//const array<int, 8> objects{
		//	_mainMatrix->at(x - 1, y - 1), // вл
		//	_mainMatrix->at(x	 , y - 1), // вв
		//	_mainMatrix->at(x + 1, y - 1), // вп
		//	_mainMatrix->at(x - 1, y	 ), // л
		//	_mainMatrix->at(x + 1, y - 1), // п
		//	_mainMatrix->at(x - 1, y + 1), // нл 
		//	_mainMatrix->at(x	 , y + 1), // нн
		//	_mainMatrix->at(x + 1, y + 1)  // нп
		//};
		
		return objects[0] + objects[1] + objects[2] + objects[3] + \
			objects[4] + objects[5] + objects[6] + objects[7];
	}
	
	int sum = 0;
	if (x > 0) {
		if (y > 0 && this->_mainMatrix->at(x - 1, y - 1)) ++sum;
		if (this->_mainMatrix->at(x - 1, y)) ++sum;
		if (y < _height - 1 && this->_mainMatrix->at(x - 1, y + 1)) ++sum;
	}

	if (this->_mainMatrix->at(x, y - 1)) ++sum;
	if (this->_mainMatrix->at(x, y + 1)) ++sum;

	if (x < _width - 1) {
		if (y > 0 && this->_mainMatrix->at(x + 1, y - 1)) ++sum;
		if (this->_mainMatrix->at(x + 1, y)) ++sum;
		if (y < _height - 1 && this->_mainMatrix->at(x + 1, y + 1)) ++sum;
	}

	return sum;
}

void CaveGenerator_flat_sum::Tick(const int count) noexcept
{
	for (int i = 0; i < count; i++) {
		
		for (size_t y = 0; y < _height; y++) {
			for (size_t x = 0; x < _width; x++) {
				int neighbours = GetNeighbours(x, y);

				bool& secondAt = this->_secondMatrix->at(x, y);
				bool& mainAt = this->_mainMatrix->at(x, y);
				
				/*secondAt = (mainAt && this->S.contains(neighbours)) ||
					(!mainAt && this->B.contains(neighbours));*/


				secondAt = (mainAt == 0) ? B.contains(neighbours) : S.contains(neighbours);
				
				/* at = (at && this->S.count(neighbours)) ||
					(!at && this->B.count(neighbours));*/
			}
		}

		std::swap(_mainMatrix, _secondMatrix);
	}
}

void CaveGenerator_flat_sum::TickMT(const int count) noexcept
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
