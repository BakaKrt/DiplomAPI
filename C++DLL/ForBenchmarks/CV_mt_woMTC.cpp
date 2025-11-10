#include "CV_mt_woMTC.h"


void CaveGenerator_mt_woMTcalc::TickMTRealization(const size_t LineFrom, const size_t LineTo) {
	size_t x = LineFrom;
	for (; x < LineTo; x++) {
		for (size_t y = 0; y < this->_width; y++) {
			int neighbours = GetNeighbours(x, y);
			this->_secondMatrix->at(x, y) = (this->_mainMatrix->at(x, y) && this->S.count(neighbours)) ||
				(!this->_mainMatrix->at(x, y) && this->B.count(neighbours));
		}
	}
}

CaveGenerator_mt_woMTcalc::CaveGenerator_mt_woMTcalc(const CaveGenerator_base& other) : CaveGenerator_base(other)
{
}

void CaveGenerator_mt_woMTcalc::Tick(int count) noexcept
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

void CaveGenerator_mt_woMTcalc::TickMT(int count) noexcept {
	static const size_t THREADS_COUNT = this->_threadsCount;
	static const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

	static vector<size_t> CHUNKS;
	CHUNKS.reserve(THREADS_COUNT * 2);

	if (CHUNKS.empty()) {
		for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
			size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

			if (LineFrom + CHUNK_SIZE > _mainMatrix->width() && LineFrom != _mainMatrix->width()) {
				LineTo = _mainMatrix->width();
			}
			CHUNKS.push_back(LineFrom);
			CHUNKS.push_back(LineTo);
		}
	}

	vector<thread> THREADS;
	THREADS.reserve(THREADS_COUNT);

	for (size_t idx = 0; idx < CHUNKS.size(); idx += 2) {
		// Убедимся, что у нас есть пара (from, to)
		if (idx + 1 < CHUNKS.size()) {
			size_t start = CHUNKS[idx];     // Значение захватывается
			size_t end = CHUNKS[idx + 1];   // Значение захватывается

			THREADS.emplace_back([this, start, end]() { // Захватываем значения start и end по значению
				this->TickMTRealization(start, end);
			});
		}
	}

	for (int i = 0; i < count; i++) {
		for (auto& th : THREADS) {
			th.join();
		}
	}
	THREADS.clear();

	// Меняем местами матрицы
	Flat2DBool* temp = _mainMatrix;
	_mainMatrix = _secondMatrix;
	_secondMatrix = temp;
}