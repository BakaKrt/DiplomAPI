#include "CaveGenerator.h"

CaveGenerator::CaveGenerator(size_t width, size_t height, bool randInit)
{
	this->Width = width;
	this->Height = height;
	this->Capacity = this->Width * this->Height;
	
	if (randInit) {
		float* array = RandomFloatCanonical(this->Capacity);
		this->MainMatrix = new Flat2DFloat(array, this->Width, this->Height);
	}
	else {
		this->MainMatrix = new Flat2DFloat(this->Width, this->Height);
	}

	this->SecondMatrix = new Flat2DFloat(this->Width, this->Height);
}

CaveGenerator::CaveGenerator(size_t width, size_t height, int threadsCount, bool randInit)
{
	this->Width = width;
	this->Height = height;
	this->Capacity = this->Width * this->Height;

	if (randInit) {
		float* array = RandomFloatCanonical(this->Capacity);
		this->MainMatrix = new Flat2DFloat(array, this->Width, this->Height);
	}
	else {
		this->MainMatrix = new Flat2DFloat(this->Width, this->Height);
	}

	this->SecondMatrix = new Flat2DFloat(this->Width, this->Height);
	this->ThreadsCount = threadsCount;
}

void CaveGenerator::SetB(std::vector<int> rulesB)
{
	this->B.clear();
	for (const auto& rule : rulesB) {
		this->B.insert(rule);
	}
}

void CaveGenerator::SetB(std::initializer_list<int> rulesB)
{
	this->B.clear();
	for (const auto& rule : rulesB) {
		this->B.insert(rule);
	}
}

void CaveGenerator::SetB(int rulesBfrom, int ruleBto)
{
	this->B.clear();

	for (int i = rulesBfrom; i < ruleBto; i++) {
		this->B.insert(i);
	}
}

void CaveGenerator::SetS(std::vector<int> rulesS)
{
	this->S.clear();
	for (const auto& rule : rulesS) {
		this->S.insert(rule);
	}
}

void CaveGenerator::SetS(std::initializer_list<int> rulesS)
{
	this->S.clear();
	for (const auto& rule : rulesS) {
		this->S.insert(rule);
	}
}

void CaveGenerator::SetS(int rulesSfrom, int ruleSto)
{
	this->S.clear();

	for (int i = rulesSfrom; i < ruleSto; i++) {
		this->S.insert(i);
	}
}

void CaveGenerator::Tick(int count)
{

}

void CaveGenerator::TickMT(int count)
{

}

void CaveGenerator::TickMTRealization(const size_t LineFrom, const size_t LineTo) {
	size_t iterator = LineFrom;
	for (; iterator < LineTo; iterator++) {
		for (size_t y = 0; y < this->Width; y++) {
			/*byte AVG = GetAVGSum(y, iterator);
			_SecondMatrix->at(y, iterator) = AVG * Koef;*/
		}
	}
}

ostream& operator<<(ostream& stream, CaveGenerator* gen)
{
	stream << "Capacity: " << gen->Capacity << std::endl;
	stream << "Width: " << gen->Width << " Height: " << gen->Height << std::endl;
	for (size_t x = 0; x < gen->Capacity;) {
		stream << gen->MainMatrix->at(x) << "\t";
		x++;
		if (x % gen->Width == 0 && x != 0) stream << '\n';
		else stream << ' ';
	}
	return stream;
}
