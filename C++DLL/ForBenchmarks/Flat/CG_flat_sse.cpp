#include "CG_flat_sse.hpp"


using namespace Benchmark;

using std::thread;



CaveGenerator_flat_sse::CaveGenerator_flat_sse(size_t width, size_t height, bool randInit) noexcept:
	CaveGeneratorBench(width, height, randInit)
{
	InitMatrix(randInit);
	name = "Flat<bool> sse";
}

CaveGenerator_flat_sse::CaveGenerator_flat_sse(size_t width, size_t height, int threadsCount, bool randInit) noexcept:
	CaveGeneratorBench(width, height, threadsCount, randInit)
{
	InitMatrix(randInit);
	InitThreads(threadsCount);
	name = "Flat<bool> sse";
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

	name = "Flat<bool> sse";
}


void CaveGenerator_flat_sse::InitMatrix(bool randInit) noexcept
{
	_mainMatrix = new Flat2DArray<bool>(_width, _height);
	_secondMatrix = new Flat2DArray<bool>(_width, _height);

	if (randInit) {
		FillArrayRandomBool<bool>(_width * _height, _mainMatrix->data());
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

int CaveGenerator_flat_sse::GetNeighbours(size_t x, size_t y) const noexcept
{
	const array<int, 8> objects{
		_mainMatrix->at(x - 1, y - 1), // вл
		_mainMatrix->at(x	 , y - 1), // вв
		_mainMatrix->at(x + 1, y - 1), // вп

		_mainMatrix->at(x - 1, y	), // л
		_mainMatrix->at(x + 1, y	), // п

		_mainMatrix->at(x - 1, y + 1), // нл
		_mainMatrix->at(x	 , y + 1), // нн
		_mainMatrix->at(x + 1, y + 1)  // нп
	};

	return sumSSE(objects);
}

int CaveGenerator_flat_sse::GetNeighboursTop(size_t x, size_t y) const noexcept
{
	const array<int, 5> objects{
		_mainMatrix->at(x - 1, y    ), // л
		_mainMatrix->at(x - 1, y + 1), // нл 
		_mainMatrix->at(x	 , y + 1), // нн
		_mainMatrix->at(x + 1, y - 1), // п
		_mainMatrix->at(x + 1, y + 1)  // нп
	};

	return objects[0] + objects[1] + objects[2] + \
		objects[3] + objects[4];
}


inline int CaveGenerator_flat_sse::GetNeighboursRight(size_t x, size_t y) const noexcept
{
	const array<int, 5> objects{
		_mainMatrix->at(x - 1, y - 1), // вл
		_mainMatrix->at(x - 1, y	), // л
		_mainMatrix->at(x - 1, y + 1), // нл 
		_mainMatrix->at(x	 , y - 1), // вв
		_mainMatrix->at(x	 , y + 1)  // нн
	};

	return objects[0] + objects[1] + objects[2] + \
		objects[3] + objects[4];
}


inline int CaveGenerator_flat_sse::GetNeighboursBottom(size_t x, size_t y) const noexcept
{
	const array<int, 5> objects{
		_mainMatrix->at(x - 1, y - 1), // вл
		_mainMatrix->at(x - 1, y	), // л
		_mainMatrix->at(x	 , y - 1), // вв
		_mainMatrix->at(x + 1, y - 1), // вп
		_mainMatrix->at(x + 1, y - 1)  // п
	};

	return objects[0] + objects[1] + objects[2] + \
		objects[3] + objects[4];
}


inline int CaveGenerator_flat_sse::GetNeighboursLeft(size_t x, size_t y) const noexcept
{
	const array<int, 5> objects{
		_mainMatrix->at(x	 , y - 1), // вв
		_mainMatrix->at(x + 1, y - 1), // вп
		_mainMatrix->at(x + 1, y),	   // п
		_mainMatrix->at(x	 , y + 1), // нн
		_mainMatrix->at(x + 1, y + 1)  // нп
	};

	return objects[0] + objects[1] + objects[2] + \
		objects[3] + objects[4];
}

/// <summary>
/// Складывает углы, возвращает сумму в порядке ВЛ, ВП, НЛ, НП
/// </summary>
inline array<int, 4> CaveGenerator_flat_sse::SumCorners() const noexcept {
	array<int, 4> res{
		// x  1   верхний левый
		// 2  3
		(int)_mainMatrix->at(1, 0) + (int)_mainMatrix->at(0, 1)  + (int)_mainMatrix->at(1, 1),

		// 1  x	  верхний правый
		// 2  3
		(int)_mainMatrix->at(_width - 2, 0) + (int)_mainMatrix->at(_width - 2, 1) + (int)_mainMatrix->at(_width - 1, 1),

		// 1 2	  нижний левый
		// x 3
		(int)_mainMatrix->at(0, _height - 2) + (int)_mainMatrix->at(1, _height - 2) + (int)_mainMatrix->at(1, _height - 1),

		// 1 2	  нижний правый
		// 3 x
		(int)_mainMatrix->at(_width - 2, _height - 2) + (int)_mainMatrix->at(_width - 1, _height - 2) + (int)_mainMatrix->at(_width - 2, _height - 1),
	};
	return res;
}

void CaveGenerator_flat_sse::Tick(const int count) noexcept
{
	const array<size_t, 8> cornerIndexes {
				 0,			  0,
		_width - 1,			  0,
				 0, _height - 1,
		_width - 1, _height - 1
	};

	for (int i = 0; i < count; i++) {

		// углы
		auto corners = SumCorners();
		size_t index_x = 0, index_y = 0;
		for (short x = 0; x < 4; x++) {
			index_x = cornerIndexes[x * 2];
			index_y = cornerIndexes[x * 2 + 1];
			this->_secondMatrix->at(index_x, index_y) = \
				(this->_mainMatrix->at(index_x, index_y) == 0) ? B.contains(corners[x]) : S.contains(corners[x]);
		}




		for (size_t y = 0; y < _height - 1; y++) {
			for (size_t x = 0; x < _width - 1; x++) {

				int neighbours = GetNeighbours(x, y);

				bool& secondAt = this->_secondMatrix->at(x, y);
				bool& mainAt = this->_mainMatrix->at(x, y);

				secondAt = (mainAt == 0) ? B.contains(neighbours) : S.contains(neighbours);
			}
		}

		std::swap(_mainMatrix, _secondMatrix);
	}
}

void CaveGenerator_flat_sse::TickMT(const int count) noexcept
{
	static const size_t THREADS_COUNT = this->_threadsCount;
	static const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

	vector<thread> THREADS;

	auto corners = SumCorners();


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
		bool& at = this->_secondMatrix->at(x, y);
		at = (at && this->S.contains(neighbours)) ||
			(!at && this->B.contains(neighbours));
		return;
	};


	if (xFrom == 0) {
		size_t xTo = lineTo;
		size_t y = 1;

		xFrom += 1;

		// внутренние рамки
		for (; xFrom < lineTo - 1; xFrom++) {
			for (; y < this->_width - 1; y++) {
				neighbours = GetNeighbours(xFrom, y);
				applyRules(xFrom, y, neighbours);
			}
		}

		// лл
		for (xFrom = 1; xFrom < lineTo; xFrom++) {
			for (y = 1; y < _width - 1; y++) {
				applyRules(xFrom, y, GetNeighboursLeft(xFrom, y));
			}
		}


		// пп
		for (xFrom = 1; xFrom < lineTo - 1; xFrom++) {
			for (y = 1; y < _width - 1; y++) {
				applyRules(xFrom, y, GetNeighboursRight(xFrom, y));
			}
		}



	}
	else if (xFrom == _height) {

	}
}

bool* CaveGenerator_flat_sse::Data() noexcept
{
	return _mainMatrix->data();
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
