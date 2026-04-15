#pragma once

#include "../CG_base.hpp"

using namespace Benchmark;

/// <summary>
/// Реализация генератора пещер с использованием плоского массива и обычным сложением
/// </summary>
class CaveGenerator_flat_sum : public CaveGeneratorBench {
protected:
	Flat2DArray<bool>* _mainMatrix = nullptr;
	Flat2DArray<bool>* _secondMatrix = nullptr;

	vector<int> _CHUNKS_INDEXES{};
public:
	CaveGenerator_flat_sum(size_t width, size_t height, bool randInit = true) noexcept;
	CaveGenerator_flat_sum(size_t width, size_t height, int threadsCount, bool randInit = true) noexcept;
	CaveGenerator_flat_sum(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept;
	
	void inline InitMatrix(bool randInit) noexcept;
	void inline InitThreads(int threadsCount) noexcept;

	int GetNeighbours(size_t x, size_t y) const noexcept override;
	
	void Tick(const int count = 1) noexcept override;
	void TickMT(const int count = 1) noexcept override;
	void TickMTRealization(const size_t lineFrom, const size_t lineTo) noexcept override;

	bool* Data() noexcept override;

	friend ostream& operator<<(ostream& stream, const CaveGenerator_flat_sum& cave) noexcept;

	~CaveGenerator_flat_sum() noexcept override;
};