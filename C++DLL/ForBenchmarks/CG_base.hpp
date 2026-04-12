#pragma once
#include <cstddef>

import std;
import FlatArray;
import Useful;

using std::unordered_set;
using std::vector, std::initializer_list;
using std::array;
using std::ostream;
using std::string;

using namespace Useful;


namespace Benchmark {
	class CaveGeneratorBench {
	protected:
		unordered_set<int> B = {};
		unordered_set<int> S = {};

		string name;

		size_t _width = 0, _height = 0;
		int _threadsCount = 2;


		virtual void TickMTRealization(const size_t LineFrom, const size_t LineTo) noexcept = 0;
		virtual int GetNeighbours(const size_t x, const size_t y) const noexcept = 0;
	public:

		CaveGeneratorBench(size_t width, size_t height, bool randInit = true) noexcept;
		CaveGeneratorBench(size_t width, size_t height, int threadsCount, bool randInit = true) noexcept;
		
		CaveGeneratorBench(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept;

		CaveGeneratorBench() noexcept;


		void SetB(vector<byte>& rulesB) noexcept;
		void SetB(initializer_list<byte> rulesB) noexcept;
		void SetB(byte rulesBfrom, byte rulesBto) noexcept;

		void SetS(vector<byte>& rulesS) noexcept;
		void SetS(initializer_list<byte> rulesS) noexcept;
		void SetS(byte rulesSfrom, byte rulesSto) noexcept;

		string getName() noexcept;

		virtual void Tick(int count = 1) noexcept = 0;
		virtual void TickMT(int count = 1) noexcept = 0;

		virtual bool* Data() noexcept = 0;

		virtual ~CaveGeneratorBench() = default;
	};
}