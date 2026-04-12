#pragma once

#include "CG_base.hpp"

using namespace Benchmark;


class CaveGenerator_vector : public CaveGeneratorBench {
protected:
	vector<vector<bool>> _mainMatrix;
	vector<vector<bool>> _secondMatrix;

	bool* _dataPtr = nullptr;
public:
	CaveGenerator_vector(size_t width, size_t height, bool randInit = true) noexcept;
	CaveGenerator_vector(size_t width, size_t height, int threadsCount, bool randInit = true) noexcept;
	CaveGenerator_vector(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept;
	
	void inline InitMatrix(bool randInit) noexcept;
	void inline InitThreads(int threadsCount) noexcept;

	int GetNeighbours(size_t x, size_t y) const noexcept override;
	
	void Tick(const int count = 1) noexcept override;
	void TickMT(const int count = 1) noexcept override;
	void TickMTRealization(const size_t lineFrom, const size_t lineTo) noexcept override;

	bool* Data() noexcept override;

	friend ostream& operator<<(ostream& stream, const CaveGenerator_vector& cave) noexcept;

	~CaveGenerator_vector() noexcept override;
};