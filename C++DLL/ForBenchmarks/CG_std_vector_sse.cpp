#include "CG_std_vector_sse.hpp"

using namespace Benchmark;

using std::thread;


CaveGenerator_vector_sse::CaveGenerator_vector_sse(size_t width, size_t height, bool randInit) noexcept:
	CaveGeneratorBench(width, height, randInit)
{
	InitMatrix(randInit);
	name = "vector<vector<bool>> sse";
}

CaveGenerator_vector_sse::CaveGenerator_vector_sse(size_t width, size_t height, int threadsCount, bool randInit) noexcept:
	CaveGeneratorBench(width, height, threadsCount, randInit)
{
	InitMatrix(randInit);
	name = "vector<vector<bool>> sse";
}

CaveGenerator_vector_sse::CaveGenerator_vector_sse(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept:
	CaveGeneratorBench(width, height, threadsCount)
{
	for (size_t x = 0; x < width; x++) {
		for (size_t y = 0; y < height; y++) {
			_mainMatrix[x][y] = vec[x + y * width];
		}
	}
}


void CaveGenerator_vector_sse::InitMatrix(bool randInit) noexcept
{
	const size_t capacity = _width * _height;

	_mainMatrix.resize(capacity);
	_secondMatrix.resize(capacity);

	for (vector<bool>& vec : _mainMatrix) {
		vec.resize(_width);
	}
	for (vector<bool>& vec : _secondMatrix) {
		vec.resize(_width);
	}


	if (randInit) {
		for (auto& line : _mainMatrix) {
			FillVectorRandomBool(line, 47);
		}

	}
}

void CaveGenerator_vector_sse::InitThreads(int threadsCount) noexcept
{
	_threadsCount = threadsCount;
}

int CaveGenerator_vector_sse::GetNeighbours(size_t x, size_t y) const noexcept
{
	if (x > 0 && x + 1 < _width && y > 0 && y + 1 < _height) {
		//const array<int, 8> objects{
		//	_mainMatrix[x - 1][y - 1], // вл
		//	_mainMatrix[x - 1][y	], // л
		//	_mainMatrix[x - 1][y + 1], // нл 
		//	_mainMatrix[x	 ][y - 1], // вв
		//	_mainMatrix[x	 ][y + 1], // нн
		//	_mainMatrix[x + 1][y - 1], // вп
		//	_mainMatrix[x + 1][y - 1], // п
		//	_mainMatrix[x + 1][y + 1]  // нп
		//};

		const array<int, 8> objects{
			_mainMatrix[x - 1][y - 1], // вл
			_mainMatrix[x	 ][y - 1], // вв
			_mainMatrix[x + 1][y - 1], // вп
			_mainMatrix[x - 1][y	], // л
			_mainMatrix[x + 1][y - 1], // п
			_mainMatrix[x - 1][y + 1], // нл 
			_mainMatrix[x	 ][y + 1], // нн
			_mainMatrix[x + 1][y + 1]  // нп
		};
		
		return sumSSE(objects);
	}
	
	int sum = 0;
	if (x > 0) {
		sum += _mainMatrix[x - 1][y];
		if (y > 0)
			sum += _mainMatrix[x - 1][y - 1];
	}
	size_t x_p1 = x + 1;
	if (y > 0) {
		sum += _mainMatrix[x][y - 1];
		if ((x > 0 && (y + 1 < _height)))
			sum += _mainMatrix[x - 1][y + 1];
		else if (x_p1 < _width) {
			sum += _mainMatrix[x_p1][y - 1];
		}
	}
	if (x_p1 < _width) {
		sum += _mainMatrix[x_p1][y];
		if (y + 1 < _height)
			sum += _mainMatrix[x_p1][y + 1];
	}
	if (y + 1 < _height) {
		sum += _mainMatrix[x][y + 1];
	}
	return sum;
}

void CaveGenerator_vector_sse::Tick(const int count) noexcept
{
	for (int i = 0; i < (int)count; i++) {

	}
}

void CaveGenerator_vector_sse::TickMT(const int count) noexcept
{
	static const size_t THREADS_COUNT = (int)this->_threadsCount;
	static const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

	static vector<size_t> _CHUNKS_INDEXES;
	_CHUNKS_INDEXES.reserve(THREADS_COUNT + 1);


	for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
		size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

		if (LineFrom + CHUNK_SIZE > _width && LineFrom != _width) {
			LineTo = _width;
		}

		if (_CHUNKS_INDEXES.size() == 0)
			_CHUNKS_INDEXES.push_back(LineFrom);

		_CHUNKS_INDEXES.push_back(LineTo);
	}

	vector<thread> THREADS;

	for (int i = 0; i < count; i++) {
		THREADS.reserve(THREADS_COUNT);

		for (size_t idx = 0; idx < _CHUNKS_INDEXES.size() - 1; idx++) {
			// пары идут в порядке [x, x+1] , [x+1, x+2], [x+2, x+3]
			if (idx + 1 < _CHUNKS_INDEXES.size()) {
				size_t start = _CHUNKS_INDEXES[idx];
				size_t end = _CHUNKS_INDEXES[idx + 1];

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

void CaveGenerator_vector_sse::TickMTRealization(const size_t lineFrom, const size_t lineTo) noexcept
{
	size_t x = lineFrom;
	for (; x < lineTo; x++) {
		for (size_t y = 0; y < this->_width; y++) {
			int neighbours = GetNeighbours(x, y);
			bool at = _secondMatrix[x][y];
			_secondMatrix[x][y] = (at && this->S.count(neighbours)) ||
				(!at && this->B.count(neighbours));
		}
	}
}

bool* CaveGenerator_vector_sse::Data() noexcept
{
	if (_dataPtr != nullptr) delete[] _dataPtr;
	_dataPtr = new bool[_width * _height];

	for (size_t y = 0; y < _height; y++) {
		for (size_t x = 0; y < _width; x++) {
			_dataPtr[x + y * _width] = _mainMatrix[x][y];
		}
	}

	return _dataPtr;
}

CaveGenerator_vector_sse::~CaveGenerator_vector_sse() noexcept {
	_width = 0;
	_height = 0;
	_threadsCount = 0;
	if (_dataPtr != nullptr) delete[] _dataPtr;
}

ostream& operator<<(ostream& stream, const CaveGenerator_vector_sse& cave) noexcept
{
	for (size_t y = 0; y < cave._height; y++) {
		for (size_t x = 0; x < cave._width; x++) {
			stream << cave._mainMatrix[x][y] << ' ';
		}
		stream << "\n";
	}

	return stream;
}
