#include "CV_base_alt_sum.h"

CaveGenerator_base_alt_calc::CaveGenerator_base_alt_calc(const CaveGenerator_base& other) : CaveGenerator_base(other)
{
}


int CaveGenerator_base_alt_calc::GetNeighbours(size_t x, size_t y)
{
	int sum = 0;

	const size_t width = _mainMatrix->width();
	const size_t height = _mainMatrix->height();


	if (x > 0 && x + 1 < width && y > 0 && y + 1 < height) {
		sum += _mainMatrix->at(x - 1, y - 1); // верхний левый
		sum += _mainMatrix->at(x, y - 1); // верхний
		sum += _mainMatrix->at(x + 1, y - 1); // верхний правый
		sum += _mainMatrix->at(x - 1, y);     // левый
		sum += _mainMatrix->at(x + 1, y);     // правый
		sum += _mainMatrix->at(x - 1, y + 1); // нижний левый
		sum += _mainMatrix->at(x, y + 1); // нижний
		sum += _mainMatrix->at(x + 1, y + 1); // нижний правый
	}
	else {
		if (x > 0) {
			sum += _mainMatrix->at(x - 1, y);
			if (y > 0)
				sum += _mainMatrix->at(x - 1, y - 1);
		}
		size_t x_p1 = x + 1;
		if (y > 0) {
			sum += _mainMatrix->at(x, y - 1);
			if (y + 1 < height)
				sum += _mainMatrix->at(x - 1, y + 1);
			else if (x_p1 < width) {
				sum += _mainMatrix->at(x_p1, y - 1);
			}
		}
		if (x_p1 < width) {
			sum += _mainMatrix->at(x_p1, y);
			if (y + 1 < height)
				sum += _mainMatrix->at(x_p1, y + 1);
		}
		if (y + 1 < height) {
			sum += _mainMatrix->at(x, y + 1);
		}
	}

	return sum;
}

void CaveGenerator_base_alt_calc::Tick(int count) noexcept
{
	for (size_t x = 0; x < this->_width; x++) {
		for (size_t y = 0; y < this->_height; y++) {
			int neighbours = GetNeighbours(x, y);

			this->_secondMatrix->at(x, y) = (this->_mainMatrix->at(x, y) && this->S.count(neighbours)) ||
				(!this->_mainMatrix->at(x, y) && this->B.count(neighbours));
		}
	}
	Flat2DBool* temp = _mainMatrix;
	_mainMatrix = _secondMatrix;
	_secondMatrix = temp;
}

void CaveGenerator_base_alt_calc::TickMT(int count) noexcept
{
	const size_t THREADS_COUNT = this->_threadsCount;
	const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

	vector<size_t> CHUNKS;
	CHUNKS.reserve(THREADS_COUNT * 2);

	for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
		size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

		if (LineFrom + CHUNK_SIZE > _mainMatrix->width() && LineFrom != _mainMatrix->width()) {
			LineTo = _mainMatrix->width();
		}
		CHUNKS.push_back(LineFrom);
		CHUNKS.push_back(LineTo);
	}

	vector<thread> THREADS;
	THREADS.reserve(THREADS_COUNT);

	for (size_t idx = 0; idx < CHUNKS.size(); idx += 2) {
		// ”бедимс€, что у нас есть пара (from, to)
		if (idx + 1 < CHUNKS.size()) {
			size_t start = CHUNKS[idx];     // «начение захватываетс€
			size_t end = CHUNKS[idx + 1];   // «начение захватываетс€

			THREADS.emplace_back([this, start, end]() { // «ахватываем значени€ start и end по значению
				this->TickMTRealization(start, end);
				});
		}
	}

	for (auto& th : THREADS) {
		th.join();
	}
	THREADS.clear();

	// ћен€ем местами матрицы
	Flat2DBool* temp = _mainMatrix;
	_mainMatrix = _secondMatrix;
	_secondMatrix = temp;
}