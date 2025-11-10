#pragma once

#include "Flat2DBool.h"
#include "Useful.h"
#include <unordered_set>

using std::unordered_set;

class CaveGenerator {
private:
	Flat2DBool* _mainMatrix   = nullptr;
	Flat2DBool* _secondMatrix = nullptr;
	size_t _width = 0, _height = 0;
	size_t _capacity = 0;

	size_t _threadsCount = 1;

	unordered_set<int> B = {};
	unordered_set<int> S = {};

	void TickMTRealization(const size_t LineFrom, const size_t LineTo);
	int GetNeighbours(size_t x, size_t y);
public:

	CaveGenerator(size_t width, size_t height, bool randInit = true);
	CaveGenerator(size_t width, size_t height, int threadsCount, bool randInit = true);
	CaveGenerator(const CaveGenerator& other);
	
	void SetB(std::vector<int> rulesB);
	void SetB(std::initializer_list<int> rulesB);
	void SetB(int rulesBfrom, int rulesBto);

	void SetS(std::vector<int> rulesS);
	void SetS(std::initializer_list<int> rulesS);
	void SetS(int rulesSfrom, int rulesSto);


	void Tick(int count = 1) noexcept;
	void TickMT(int count = 1) noexcept;

	friend ostream& operator<<(ostream& stream, CaveGenerator* gen);
};