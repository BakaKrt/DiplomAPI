#include "CaveGenerator.h"

CaveGenerator::CaveGenerator(size_t width, size_t height, bool randInit)
{
	this->Width = width;
	this->Height = height;
	this->Capacity = this->Width * this->Height;
	
	if (randInit) {
		bool* array = RandomBoolArray(this->Capacity);
		this->MainMatrix = new Flat2DBool(array, this->Width, this->Height);
	}
	else {
		this->MainMatrix = new Flat2DBool(this->Width, this->Height);
	}

	this->SecondMatrix = new Flat2DBool(this->Width, this->Height);
}

CaveGenerator::CaveGenerator(size_t width, size_t height, int threadsCount, bool randInit)
{
	this->Width = width;
	this->Height = height;
	this->Capacity = this->Width * this->Height;

	if (randInit) {
		bool* array = RandomBoolArray(this->Capacity);
		this->MainMatrix = new Flat2DBool(array, this->Width, this->Height);
	}
	else {
		this->MainMatrix = new Flat2DBool(this->Width, this->Height);
	}

	this->SecondMatrix = new Flat2DBool(this->Width, this->Height);
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

int CaveGenerator::GetNeighbours(size_t x, size_t y)
{
	using std::cout;
	using std::endl;

	int neighboursCount = 0;
	size_t x_m = 0, y_m = 0;
	if (x == 0) x_m = 0;
	else x_m = x - 1;

	if (y == 0) y_m = 0;
	else y_m = y - 1;

	for (size_t iter_x = x_m; iter_x <= x + 1; iter_x++) {
		if (iter_x >= this->Width) {
			//cout << "iter_x " << iter_x << " >= " << this->Width << ", continue\n";
			continue;
		}
		for (size_t iter_y = y_m; iter_y <= y + 1; iter_y++) {
			if (iter_y >= this->Height) {
				//cout << "iter_y " << iter_y << " >= " << this->Height << ", continue\n";
				continue;
			}

			if (iter_x == x && iter_y == y) continue;

			if (this->MainMatrix->at(iter_x, iter_y) == true)
				neighboursCount++;
		}
	}
	return neighboursCount;
}

void CaveGenerator::Tick(int count)
{
	for (size_t x = 0; x < this->Width; x++) {
		for (size_t y = 0; y < this->Height; y++) {
			int neighbours = GetNeighbours(x , y);

			if (this->MainMatrix->at(x, y)) {
				if (this->S.count(neighbours)) {
					this->SecondMatrix->at(x, y) = true;
				}
				else {
					this->SecondMatrix->at(x, y) = false;
				}
			}
			else {
				if (this->B.count(neighbours)) {
					this->SecondMatrix->at(x, y) = true;
				}
				else {
					this->SecondMatrix->at(x, y) = false;
				}
			}
		}
	}
	Flat2DBool* temp = MainMatrix;
	MainMatrix = SecondMatrix;
	SecondMatrix = temp;
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
