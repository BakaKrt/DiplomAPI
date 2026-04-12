#include <cstdint>
#include <string>

export module FlatArray;

import std;
import SharedMemoryHelper;

using namespace SharedMemory;

using std::ostream;
using byte = std::uint8_t;
using std::make_unique, std::unique_ptr;

// Список разрешённых типов
export template<typename T>
struct is_allowed_type : std::integral_constant<bool,
	std::is_same_v<T, int> ||
	std::is_same_v<T, float> ||
	std::is_same_v<T, bool>
> {
};


export 
template<typename T, typename = std::enable_if_t<is_allowed_type<T>::value>>
/// <summary>
/// Использует разделяемую память для создания плоского массива с интерфейсом двумерного
/// Измерения:
///     w  i  d  t  h -> (x)
/// h   0  1  2  3  4
/// e   5  6  7  8  9
/// i  10 11 12 13 14
/// g  15 16 17 18 19
/// h  20 21 22 23 24
/// t  25 26 27 28 29
/// |  30 31 32 33 34
/// ↓  35 36 37 38 39
/// (y)
/// </summary>
class Flat2DArray {
private:
	unique_ptr<SharedMemoryObject> _object;
    size_t _width = 0, _height = 0;
	T* _array = nullptr;
	friend class HeightMap;
public:
    Flat2DArray() noexcept;

	Flat2DArray(size_t width, size_t height) noexcept;

    /// <summary>
    /// Конструктор копирования
    /// </summary>
    /// <param name="other"></param>
    Flat2DArray(const Flat2DArray& other) noexcept;

	Flat2DArray& operator=(const Flat2DArray& other);

    inline T& at(size_t x) const noexcept;
    inline T& at(size_t x, size_t y) const noexcept;

	inline T& operator[] (size_t posX) noexcept;
	inline const T& operator[] (size_t posX) const noexcept;

    T* data() const noexcept;
    size_t capacity() const noexcept;
    size_t width() const noexcept;
    size_t height() const noexcept;

	friend ostream& operator <<(ostream& stream, Flat2DArray& data);

	~Flat2DArray() noexcept;
};


template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray() noexcept :
	_width(0), _height(0)
{

}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray(size_t width, size_t height) noexcept :
	_width(width),
	_height(height)
{
	this->_object = make_unique<SharedMemoryObject>(this->_width * this->_height, sizeof(T));

	auto settings = _object->create();
	this->_array = static_cast<T*>(settings.array);
}


template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray(const Flat2DArray<T, Enabled>& other) noexcept
{
	_width = other._width;
	_height = other._height;


	wstring memoryName = other._object->getName();

	this->_object = make_unique<SharedMemoryObject>(memoryName);
	auto settings = _object->connect();

	this->_array = static_cast<T*>(settings.array);
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>& Flat2DArray<T, Enabled>::operator=(const Flat2DArray& other)
{
	exit(-1);
	throw std::exception("допилить надо");
	if (this != &other) { // Проверка на самоприсваивание
		delete[] _array; // Освобождаем старую память

		_width = other._width;
		_height = other._height;
		const size_t capacity = _width * _height;

		if (other._array) {
			_array = new T[capacity];
			std::memcpy(_array, other._array, capacity);
		}
		else {
			_array = nullptr;
		}
	}
	return *this;
}

template<typename T, typename Enabled>
inline T& Flat2DArray<T, Enabled>::at(size_t x) const noexcept
{
	return _array[x];
}

template<typename T, typename Enabled>
inline T& Flat2DArray<T, Enabled>::at(size_t x, size_t y) const noexcept
{
	return _array[x + y * _width];
}

template<typename T, typename Enabled>
inline T& Flat2DArray<T, Enabled>::operator[](size_t posX) noexcept
{
	return _array[posX];
}

template<typename T, typename Enabled>
inline const T& Flat2DArray<T, Enabled>::operator[](size_t posX) const noexcept
{
	return _array[posX];
}

template<typename T, typename Enabled>
T* Flat2DArray<T, Enabled>::data() const noexcept
{
	return _array;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::capacity() const noexcept
{
	return this->_width * this->_height;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::width() const noexcept
{
	return _width;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::height() const noexcept
{
	return _height;
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::~Flat2DArray() noexcept
{
	_array = nullptr;
	_width = 0; _height = 0;
}

template<typename T, typename Enabled>
ostream& operator<<(ostream& stream, Flat2DArray<T, Enabled>& data)
{
	const size_t capacity = data._width * data._height;
	for (size_t i = 0; i < capacity;)
	{
		stream << (int)data.at(i) << "\t";
		i++;
		if (i % data._width == 0 && i != 0) stream << '\n';
		else stream << ' ';
	}
	return stream;
}
