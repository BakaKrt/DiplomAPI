#include "CaveGenerator.h"

CaveGenerator::CaveGenerator(size_t width, size_t height, bool randInit)
{
	this->_width = width;
	this->_height = height;
	this->_capacity = this->_width * this->_height;
	
	if (randInit) {
		bool* array = RandomBoolArray(this->_capacity);
		this->_mainMatrix = new Flat2DBool(array, this->_width, this->_height);
	}
	else {
		this->_mainMatrix = new Flat2DBool(this->_width, this->_height);
	}

	this->_secondMatrix = new Flat2DBool(this->_width, this->_height);
}

CaveGenerator::CaveGenerator(size_t width, size_t height, int threadsCount, bool randInit)
{
	this->_width = width;
	this->_height = height;
	this->_capacity = this->_width * this->_height;

	if (randInit) {
		bool* array = RandomBoolArray(this->_capacity);
		this->_mainMatrix = new Flat2DBool(array, this->_width, this->_height);
	}
	else {
		this->_mainMatrix = new Flat2DBool(this->_width, this->_height);
	}

	this->_secondMatrix = new Flat2DBool(this->_width, this->_height);
	this->_threadsCount = GetThreadsCount(threadsCount);
}

CaveGenerator::CaveGenerator(const CaveGenerator& other) {
	this->_width = other._width;
	this->_height = other._height;
	this->_capacity = this->_width * this->_height;

	this->B = other.B;
	this->S = other.S;

	this->_threadsCount = other._threadsCount;

	this->_mainMatrix = new Flat2DBool(*other._mainMatrix);
	this->_secondMatrix = new Flat2DBool(*other._secondMatrix);
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
		if (iter_x >= this->_width) {
			//cout << "iter_x " << iter_x << " >= " << this->Width << ", continue\n";
			continue;
		}
		for (size_t iter_y = y_m; iter_y <= y + 1; iter_y++) {
			if (iter_y >= this->_height) {
				//cout << "iter_y " << iter_y << " >= " << this->Height << ", continue\n";
				continue;
			}

			if (iter_x == x && iter_y == y) continue;

			if (this->_mainMatrix->at(iter_x, iter_y) == true)
				neighboursCount++;
		}
	}
	return neighboursCount;
}

void CaveGenerator::Tick(int count) noexcept
{
	for (size_t x = 0; x < this->_width; x++) {
		for (size_t y = 0; y < this->_height; y++) {
			int neighbours = GetNeighbours(x , y);

			if (this->_mainMatrix->at(x, y)) {
				if (this->S.count(neighbours)) {
					this->_secondMatrix->at(x, y) = true;
				}
				else {
					this->_secondMatrix->at(x, y) = false;
				}
			}
			else {
				if (this->B.count(neighbours)) {
					this->_secondMatrix->at(x, y) = true;
				}
				else {
					this->_secondMatrix->at(x, y) = false;
				}
			}
		}
	}
	Flat2DBool* temp = _mainMatrix;
	_mainMatrix = _secondMatrix;
	_secondMatrix = temp;
}

void CaveGenerator::TickMT(int count) noexcept
{
	static const size_t THREADS_COUNT = this->_threadsCount;
	static const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

	static vector<size_t> CHUNKS;
	CHUNKS.reserve(THREADS_COUNT * 2);

	if (CHUNKS.empty()) {
		for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
			size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

			if (LineFrom + CHUNK_SIZE > _mainMatrix->Width && LineFrom != _mainMatrix->Width) {
				LineTo = _mainMatrix->Width;
			}
			CHUNKS.push_back(LineFrom);
			CHUNKS.push_back(LineTo);
		}
	}

	vector<thread> THREADS;
	THREADS.reserve(THREADS_COUNT);

	for (size_t idx = 0; idx < CHUNKS.size(); idx += 2) {
		// Убедимся, что у нас есть пара (from, to)
		if (idx + 1 < CHUNKS.size()) {
			size_t start = CHUNKS[idx];     // Значение захватывается
			size_t end = CHUNKS[idx + 1];   // Значение захватывается

			THREADS.emplace_back([this, start, end]() { // Захватываем значения start и end по значению
				this->TickMTRealization(start, end);
				});
		}
	}

	for (int i = 0; i < count; i++) {
		for (auto& th : THREADS) {
			th.join();
		}
		THREADS.clear();
	}

	// Меняем местами матрицы
	Flat2DBool* temp = _mainMatrix;
	_mainMatrix = _secondMatrix;
	_secondMatrix = temp;
}

void CaveGenerator::TickMTRealization(const size_t LineFrom, const size_t LineTo) {
	size_t x = LineFrom;
	for (; x < LineTo; x++) {
		for (size_t y = 0; y < this->_width; y++) {
			int count = GetNeighbours(x, y);
			this->_secondMatrix->at(x, y) = count;
		}
	}
}

ostream& operator<<(ostream& stream, CaveGenerator* gen)
{
	stream << "Capacity: " << gen->_capacity << std::endl;
	stream << "Width: " << gen->_width << " Height: " << gen->_height << std::endl;
	for (size_t x = 0; x < gen->_capacity;) {
		stream << gen->_mainMatrix->at(x) << "\t";
		x++;
		if (x % gen->_width == 0 && x != 0) stream << '\n';
		else stream << ' ';
	}
	return stream;
}
