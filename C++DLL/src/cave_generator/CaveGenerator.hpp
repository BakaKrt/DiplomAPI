#pragma once

import std;
import FlatArray;
import Useful;

using std::unordered_set;
using std::vector;
using std::array;
using std::ostream;

using namespace Useful;

class CaveGenerator {
private:
	Flat2DArray<bool>* _mainMatrix   = nullptr;
	Flat2DArray<bool>* _secondMatrix = nullptr;

	vector<int> _CHUNKS_INDEXES{};

	size_t _width = 0, _height = 0;
	size_t _capacity = 0;

	int _threadsCount = 2;

	unordered_set<int> B = {};
	unordered_set<int> S = {};

	void TickMTRealization(const size_t LineFrom, const size_t LineTo);
	int GetNeighbours(size_t x, size_t y);
public:

	CaveGenerator(size_t width, size_t height, bool randInit = true) noexcept;
	CaveGenerator(size_t width, size_t height, int threadsCount, bool randInit = true) noexcept;
	CaveGenerator(const CaveGenerator& other) noexcept;
	

	void SetB(vector<byte>& rulesB);
	void SetB(std::initializer_list<byte> rulesB);
	void SetB(byte rulesBfrom, byte rulesBto);

	void SetS(vector<byte>& rulesS);
	void SetS(std::initializer_list<byte> rulesS);
	void SetS(byte rulesSfrom, byte rulesSto);


	void Tick(int count = 1) noexcept;
	void TickMT(int count = 1) noexcept;

	friend std::ostream& operator<<(std::ostream& stream, CaveGenerator& gen);

	~CaveGenerator();
};