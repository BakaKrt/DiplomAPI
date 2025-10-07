#pragma once

#include "Flat2DFloat.h"
#include <unordered_set>

using std::unordered_set;

class CaveGenerator {
private:
	Flat2DFloat* MainMatrix   = nullptr;
	Flat2DFloat* SecondMatrix = nullptr;
	size_t Width = 0, Height = 0;
	size_t Capacity = 0;

	unordered_set<int> B = {};
	unordered_set<int> S = {};
public:

	CaveGenerator(size_t width, size_t height, bool randInit = true);
	
	void SetB(std::vector<int> rulesB);
	void SetB(std::initializer_list<int> rulesB);
	void SetB(int rulesBfrom, int ruleBto);
	void SetS(std::vector<int> rulesS);
	void SetS(std::initializer_list<int> rulesS);
	void SetS(int rulesSfrom, int ruleSto);


	void Tick(int count);
	void TickMT(int count);
};