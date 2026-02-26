#include "CaveGenerator.hpp"

using std::thread;

CaveGenerator::CaveGenerator(size_t width, size_t height, bool randInit)
{
	this->_width = width;
	this->_height = height;
	this->_capacity = this->_width * this->_height;
	
	if (randInit) {
		bool* array = RandomBoolArray(this->_capacity);
		this->_mainMatrix = new Flat2DArray<bool>(array, this->_width, this->_height);
	}
	else {
		this->_mainMatrix = new Flat2DArray<bool>(this->_width, this->_height);
	}

	this->_secondMatrix = new Flat2DArray<bool>(this->_width, this->_height);
	this->_threadsCount = 2;
}

CaveGenerator::CaveGenerator(size_t width, size_t height, int threadsCount, bool randInit)
{
	this->_width = width;
	this->_height = height;
	this->_capacity = this->_width * this->_height;

	if (randInit) {
		bool* array = RandomBoolArray(this->_capacity);
		this->_mainMatrix = new Flat2DArray<bool>(array, this->_width, this->_height);
	}
	else {
		this->_mainMatrix = new Flat2DArray<bool>(this->_width, this->_height);
	}

	this->_secondMatrix = new Flat2DArray<bool>(this->_width, this->_height);
	this->_threadsCount = GetThreadsCount(threadsCount);

	const size_t CHUNK_SIZE = (this->_height + _threadsCount - 1) / _threadsCount;

	_CHUNKS.reserve(_threadsCount + 1);

	const size_t mainMatrixWidth = _mainMatrix->width();

	for (size_t i = 0; i < this->_height; i += CHUNK_SIZE) {
		size_t LineFrom = i, LineTo = LineFrom + CHUNK_SIZE;

		if (LineFrom + CHUNK_SIZE > mainMatrixWidth && LineFrom != mainMatrixWidth) {
			LineTo = mainMatrixWidth;
		}

		if (_CHUNKS.size() == 0)
			_CHUNKS.push_back((int)LineFrom);

		_CHUNKS.push_back((int)LineTo);
	}
}

CaveGenerator::CaveGenerator(const CaveGenerator& other) {
	this->_width = other._width;
	this->_height = other._height;
	this->_capacity = this->_width * this->_height;

	this->B = other.B;
	this->S = other.S;

	this->_threadsCount = other._threadsCount;

	this->_mainMatrix = new Flat2DArray<bool>(*other._mainMatrix);
	this->_secondMatrix = new Flat2DArray<bool>(*other._secondMatrix);
}

void CaveGenerator::SetB(vector<byte>& rulesB)
{
	this->B.clear();
	for (const auto& rule : rulesB) {
		this->B.insert(rule);
	}
}

void CaveGenerator::SetB(std::initializer_list<byte> rulesB)
{
	this->B.clear();
	for (const auto& rule : rulesB) {
		this->B.insert(rule);
	}
}

void CaveGenerator::SetB(byte rulesBfrom, byte ruleBto)
{
	this->B.clear();

	for (int i = rulesBfrom; i < ruleBto; i++) {
		this->B.insert(i);
	}
}

void CaveGenerator::SetS(vector<byte>& rulesS)
{
	this->S.clear();
	for (const auto& rule : rulesS) {
		this->S.insert(rule);
	}
}

void CaveGenerator::SetS(std::initializer_list<byte> rulesB)
{
	this->B.clear();
	for (const auto& rule : rulesB) {
		this->B.insert(rule);
	}
}

void CaveGenerator::SetS(byte rulesSfrom, byte ruleSto)
{
	this->S.clear();

	for (int i = rulesSfrom; i < ruleSto; i++) {
		this->S.insert(i);
	}
}


int CaveGenerator::GetNeighbours(size_t x, size_t y)
{
	int sum = 0;

	const size_t width = _mainMatrix->width();
	const size_t height = _mainMatrix->height();


	if (x > 0 && x + 1 < width && y > 0 && y + 1 < height) {
		sum += _mainMatrix->at(x - 1, y - 1); // верхний левый
		sum += _mainMatrix->at(x, y - 1);	  // верхний
		sum += _mainMatrix->at(x + 1, y - 1); // верхний правый
		sum += _mainMatrix->at(x - 1, y);     // левый
		sum += _mainMatrix->at(x + 1, y);     // правый
		sum += _mainMatrix->at(x - 1, y + 1); // нижний левый
		sum += _mainMatrix->at(x, y + 1);	  // нижний
		sum += _mainMatrix->at(x + 1, y + 1); // нижний правый
	}
	else {
		if (x > 0) {
			sum += _mainMatrix->at(x - 1, y);
			if (y > 0)
				sum += _mainMatrix->at(x - 1, y - 1);
		}
		size_t x_p1 = x + 1;
		if (y > 0) {
			sum += _mainMatrix->at(x, y - 1);
			if (y + 1 < height)
				sum += _mainMatrix->at(x - 1, y + 1);
			else if (x_p1 < width) {
				sum += _mainMatrix->at(x_p1, y - 1);
			}
		}
		if (x_p1 < width) {
			sum += _mainMatrix->at(x_p1, y);
			if (y + 1 < height)
				sum += _mainMatrix->at(x_p1, y + 1);
		}
		if (y + 1 < height) {
			sum += _mainMatrix->at(x, y + 1);
		}
	}

	return sum;
}

void CaveGenerator::Tick(int count) noexcept
{
	for (size_t x = 0; x < this->_width; x++) {
		for (size_t y = 0; y < this->_height; y++) {
			int neighbours = GetNeighbours(x, y);

			this->_secondMatrix->at(x, y) = (this->_mainMatrix->at(x, y) && this->S.count(neighbours)) ||
				(!this->_mainMatrix->at(x, y) && this->B.count(neighbours));
		}
	}

	std::swap(_mainMatrix, _secondMatrix);
}

void CaveGenerator::TickMT(int count) noexcept
{
	static const size_t THREADS_COUNT = this->_threadsCount;
	static const size_t CHUNK_SIZE = (this->_height + THREADS_COUNT - 1) / THREADS_COUNT;

	vector<thread> THREADS;
	
	for (int i = 0; i < count; i++) {
		THREADS.reserve(THREADS_COUNT);

		for (size_t idx = 0; idx < this->_CHUNKS.size() - 1; idx++) {
			// пары идут в порядке [x, x+1] , [x+1, x+2], [x+2, x+3]
			if (idx + 1 < _CHUNKS.size()) {
				size_t start = _CHUNKS[idx];
				size_t end = _CHUNKS[idx + 1];

				THREADS.emplace_back([this, start, end]() {
					this->TickMTRealization(start, end);
				});
			}
		}

		for (auto& th : THREADS) {
			th.join();
		}
		THREADS.clear();

		// Меняем местами матрицы
		std::swap(_mainMatrix, _secondMatrix);
	}
}

void CaveGenerator::TickMTRealization(const size_t LineFrom, const size_t LineTo) {
	size_t x = LineFrom;
	for (; x < LineTo; x++) {
		for (size_t y = 0; y < this->_width; y++) {
			int neighbours = GetNeighbours(x, y);
			this->_secondMatrix->at(x, y) = (this->_mainMatrix->at(x, y) && this->S.count(neighbours)) ||
				(!this->_mainMatrix->at(x, y) && this->B.count(neighbours));
		}
	}
}

std::ostream& operator<<(std::ostream& stream, CaveGenerator& gen)
{
	stream << "Capacity: " << gen._capacity << std::endl;
	stream << "Width: " << gen._width << " Height: " << gen._height << std::endl;
	for (size_t x = 0; x < gen._capacity;) {
		stream << gen._mainMatrix->at(x) << "\t";
		x++;
		if (x % gen._width == 0 && x != 0) stream << '\n';
		else stream << ' ';
	}
	return stream;
}


CaveGenerator::~CaveGenerator()
{
	delete _mainMatrix;
	delete _secondMatrix;
	_width = 0; _height = 0;
	_capacity = 0;
	_threadsCount = 0;
}