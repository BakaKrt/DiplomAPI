#pragma once

#include "../CG_base.hpp"

using namespace Benchmark;

using std::unique_ptr;
using std::make_unique;

using std::array;

class CaveGenerator_flat_sse : public CaveGeneratorBench {
protected:
	Flat2DArray<bool>* _mainMatrix = nullptr;
	Flat2DArray<bool>* _secondMatrix = nullptr;

	vector<int> _CHUNKS_INDEXES{};
public:
	CaveGenerator_flat_sse(size_t width, size_t height, bool randInit = true) noexcept;
	CaveGenerator_flat_sse(size_t width, size_t height, int threadsCount, bool randInit = true) noexcept;
	CaveGenerator_flat_sse(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept;
	
	void inline InitMatrix(bool randInit) noexcept;
	void inline InitThreads(int threadsCount) noexcept;

	int GetNeighbours(size_t x, size_t y) const noexcept override;
	inline int GetNeighboursTop(size_t x, size_t y) const noexcept;
	inline int GetNeighboursRight(size_t x, size_t y) const noexcept;
	inline int GetNeighboursBottom(size_t x, size_t y) const noexcept;
	inline int GetNeighboursLeft(size_t x, size_t y) const noexcept;
	inline array<int, 4> SumCorners() const noexcept;
	
	void Tick(const int count = 1) noexcept override;
	void TickMT(const int count = 1) noexcept override;
	void TickMTRealization(const size_t lineFrom, const size_t lineTo) noexcept override;

	bool* Data() noexcept override;

	friend ostream& operator<<(ostream& stream, const CaveGenerator_flat_sse& cave) noexcept;

	~CaveGenerator_flat_sse() noexcept override;
};