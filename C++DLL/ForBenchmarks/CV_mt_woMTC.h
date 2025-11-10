#pragma once

#include "CV_base.h"

using std::unordered_set;

class CaveGenerator_mt_woMTcalc: public CaveGenerator_base {
private:

	void TickMTRealization(const size_t LineFrom, const size_t LineTo) override;
public:
	CaveGenerator_mt_woMTcalc(const CaveGenerator_base& other);

	void Tick(int count = 1) noexcept override;
	void TickMT(int count = 1) noexcept override;
};