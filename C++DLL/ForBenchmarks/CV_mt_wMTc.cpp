#include "CV_mt_wMTC.h"


//void CaveGenerator_mt_wMTcalc::TickMTRealization(const size_t LineFrom, const size_t LineTo) {
//	size_t x = LineFrom;
//	for (; x < LineTo; x++) {
//		for (size_t y = 0; y < this->_width; y++) {
//			int neighbours = GetNeighbours(x, y);
//			this->_secondMatrix->at(x, y) = (this->_mainMatrix->at(x, y) && this->S.count(neighbours)) ||
//				(!this->_mainMatrix->at(x, y) && this->B.count(neighbours));
//		}
//	}
//}

CaveGenerator_mt_wMTcalc::CaveGenerator_mt_wMTcalc(const CaveGenerator_base& other) : CaveGenerator_base(other)
{
	const size_t CHUNK_SIZE = (this->_height + _threadsCount - 1) / _threadsCount;

	_CHUNKS.reserve(_threadsCount * 2);

	for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
		size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

		if (LineFrom + CHUNK_SIZE > _mainMatrix->width() && LineFrom != _mainMatrix->width()) {
			LineTo = _mainMatrix->width();
		}
		_CHUNKS.push_back(LineFrom);
		_CHUNKS.push_back(LineTo);
	}
}

void CaveGenerator_mt_wMTcalc::Tick(int count) noexcept
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

void CaveGenerator_mt_wMTcalc::TickMT(int count) noexcept {
	static const size_t THREADS_COUNT = this->_threadsCount;

	vector<thread> THREADS;
	THREADS.reserve(THREADS_COUNT);

	for (size_t idx = 0; idx < _CHUNKS.size(); idx += 2) {
		if (idx + 1 < _CHUNKS.size()) {
			size_t start = _CHUNKS[idx];     // Значение захватывается
			size_t end = _CHUNKS[idx + 1];   // Значение захватывается

			THREADS.emplace_back([this, start, end]() { // Захватываем значения start и end по значению
				this->TickMTRealization(start, end);
			});
		}
	}

	for (auto& th : THREADS) {
		th.join();
	}
	THREADS.clear();

	// Меняем местами матрицы
	Flat2DBool* temp = _mainMatrix;
	_mainMatrix = _secondMatrix;
	_secondMatrix = temp;
}