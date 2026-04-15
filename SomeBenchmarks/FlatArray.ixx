#include <cstdint>

export module Flat2DArray;

import std;
import SharedMemoryHelper;

using namespace SharedMemory;

using std::wstring;

using std::ostream;
using byte = std::uint8_t;
using std::make_unique, std::unique_ptr;

// Список разрешённых типов
export template<typename T>
concept allowed_type =
	std::is_same_v<T, int> ||
	std::is_same_v<T, float> ||
	std::is_same_v<T, uint8_t> ||
	std::is_same_v<T, bool>
;


export
template<typename T> requires allowed_type<T>
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
public:
	Flat2DArray() noexcept :
		_width(0), _height(0), _object(nullptr) {

	}

	Flat2DArray(size_t width, size_t height) noexcept :
		_width(width), _height(height),
		_array(nullptr)
	{
		this->_object = make_unique<SharedMemoryObject>(this->_width * this->_height, sizeof(T));

		auto settings = _object->create();
		this->_array = static_cast<T*>(settings.array);
	}

    /// <summary>
    /// Конструктор копирования
    /// </summary>
    /// <param name="other"></param>
	Flat2DArray(const Flat2DArray<T>& other) noexcept :
		_width(other._width), _height(other._height),
		_array(nullptr)
	{
		wstring memoryName = other._object->getName();
		this->_object = make_unique<SharedMemoryObject>(memoryName);
		auto settings = _object->connect();
		this->_array = static_cast<T*>(settings.array);
	}

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


template<typename T> requires allowed_type<T>
inline T& Flat2DArray<T>::at(size_t x) const noexcept
{
	return _array[x];
}

template<typename T> requires allowed_type<T>
inline T& Flat2DArray<T>::at(size_t x, size_t y) const noexcept
{
	return _array[x + y * _width];
}

template<typename T> requires allowed_type<T>
inline T& Flat2DArray<T>::operator[](size_t posX) noexcept
{
	return _array[posX];
}

template<typename T> requires allowed_type<T>
inline const T& Flat2DArray<T>::operator[](size_t posX) const noexcept
{
	return _array[posX];
}

template<typename T> requires allowed_type<T>
T* Flat2DArray<T>::data() const noexcept
{
	return _array;
}

template<typename T> requires allowed_type<T>
size_t Flat2DArray<T>::capacity() const noexcept
{
	return this->_width * this->_height;
}

template<typename T> requires allowed_type<T>
size_t Flat2DArray<T>::width() const noexcept
{
	return _width;
}

template<typename T> requires allowed_type<T>
size_t Flat2DArray<T>::height() const noexcept
{
	return _height;
}

template<typename T> requires allowed_type<T>
Flat2DArray<T>::~Flat2DArray() noexcept
{
	_array = nullptr;
	_width = 0; _height = 0;
}

template<typename T> requires allowed_type<T>
ostream& operator<<(ostream& stream, Flat2DArray<T>& data)
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
