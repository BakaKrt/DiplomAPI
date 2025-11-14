#pragma once

#include "CV_base.h"

using std::unordered_set;

class CaveGenerator_base_alt_calc : public CaveGenerator_base {
private:

	//void TickMTRealization(const size_t LineFrom, const size_t LineTo) override;
	int GetNeighbours(size_t x, size_t y) override;
public:
	CaveGenerator_base_alt_calc(const CaveGenerator_base& other);

	void Tick(int count = 1) noexcept override;
	void TickMT(int count = 1) noexcept override;
};