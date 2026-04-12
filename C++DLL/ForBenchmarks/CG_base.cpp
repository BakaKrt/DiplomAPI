#include "CG_base.hpp"

void Benchmark::CaveGeneratorBench::SetB(vector<byte>& rulesB) noexcept
{
	this->B.clear();
	for (const auto& rule : rulesB) {
		this->B.insert(rule);
	}
}

void Benchmark::CaveGeneratorBench::SetB(std::initializer_list<byte> rulesB) noexcept
{
	this->B.clear();
	for (const auto& rule : rulesB) {
		this->B.insert(rule);
	}
}

void Benchmark::CaveGeneratorBench::SetB(byte rulesBfrom, byte rulesBto) noexcept
{

	this->B.clear();

	for (int i = rulesBfrom; i < rulesBto; i++) {
		this->B.insert(i);
	}
}

void Benchmark::CaveGeneratorBench::SetS(vector<byte>& rulesS) noexcept
{
	this->S.clear();
	for (const auto& rule : rulesS) {
		this->S.insert(rule);
	}
}

void Benchmark::CaveGeneratorBench::SetS(std::initializer_list<byte> rulesS) noexcept
{
	this->S.clear();
	for (const auto& rule : rulesS) {
		this->S.insert(rule);
	}
}

void Benchmark::CaveGeneratorBench::SetS(byte rulesSfrom, byte rulesSto) noexcept
{
	this->S.clear();

	for (int i = rulesSfrom; i < rulesSto; i++) {
		this->S.insert(i);
	}
}

string Benchmark::CaveGeneratorBench::getName() noexcept
{
	return this->name;
}

Benchmark::CaveGeneratorBench::CaveGeneratorBench(size_t width, size_t height, bool randInit) noexcept:
	_width(width), _height(height)
{
	B = { 3 };
	S = { 2, 3 };
}

Benchmark::CaveGeneratorBench::CaveGeneratorBench(size_t width, size_t height, int threadsCount, bool randInit) noexcept:
	Benchmark::CaveGeneratorBench(width, height, randInit)
{
	_threadsCount = threadsCount;
	B = { 3 };
	S = { 2, 3 };
}

Benchmark::CaveGeneratorBench::CaveGeneratorBench(size_t width, size_t height, vector<bool>& vec, int threadsCount) noexcept:
	_width(width), _height(height), _threadsCount(threadsCount)
{
	B = { 3 };
	S = { 2, 3 };
}


Benchmark::CaveGeneratorBench::CaveGeneratorBench() noexcept:
	_width(0), _height(0), _threadsCount(2)
{
	B = { 3 };
	S = { 2, 3 };
}