#pragma once

#include <unordered_set>
#include "../Flat2DBool.h"
#include "../Useful.h"

using std::unordered_set;

class CaveGenerator_base {
protected:
	Flat2DBool* _mainMatrix = nullptr;
	Flat2DBool* _secondMatrix = nullptr;
	size_t _width = 0, _height = 0;
	size_t _capacity = 0;

	size_t _threadsCount = 1;

	unordered_set<int> B = {};
	unordered_set<int> S = {};

	virtual void TickMTRealization(const size_t LineFrom, const size_t LineTo);
	virtual int GetNeighbours(size_t x, size_t y);
public:

	CaveGenerator_base(size_t width, size_t height, bool randInit = true);
	CaveGenerator_base(size_t width, size_t height, int threadsCount, bool randInit = true);
	CaveGenerator_base(const CaveGenerator_base& other);
	CaveGenerator_base();

	void SetB(std::vector<int> rulesB);
	void SetB(std::initializer_list<int> rulesB);
	void SetB(int rulesBfrom, int rulesBto);

	void SetS(std::vector<int> rulesS);
	void SetS(std::initializer_list<int> rulesS);
	void SetS(int rulesSfrom, int rulesSto);


	virtual void Tick(int count = 1) noexcept;
	virtual void TickMT(int count = 1) noexcept;

	friend ostream& operator<<(ostream& stream, CaveGenerator_base* gen);
};