#pragma once

#include <cstdint>
#include <string>

import std;
import SharedMemoryHelper;

using namespace SharedMemory;

using std::ostream;
using byte = std::uint8_t;
using std::make_unique, std::unique_ptr;

// Список разрешённых типов
template<typename T>
struct is_allowed_type : std::integral_constant<bool,
    std::is_same_v<T, int> ||
    std::is_same_v<T, float> ||
    std::is_same_v<T, bool>
> {
};


template<typename T, typename = std::enable_if_t<is_allowed_type<T>::value>>
class Flat2DArray {
private:
	unique_ptr<SharedMemoryObject> _object;
    size_t _width, _height;
	T* _array = nullptr;
    friend class HeightMap;
public:
    Flat2DArray();

    Flat2DArray(size_t width, size_t height) noexcept;

    //Flat2DArray(T* array, size_t width, size_t height) noexcept;

    /// <summary>
    /// Конструктор копирования
    /// </summary>
    /// <param name="other"></param>
    Flat2DArray(const Flat2DArray& other);

    Flat2DArray& operator=(const Flat2DArray& other);

    T& at(size_t x);

    T& at(size_t x, size_t y);

    T& operator[] (size_t posX) noexcept;

    T* data() const noexcept;
    size_t capacity() const;
    size_t width() const;
    size_t height() const;

    friend ostream& operator <<(ostream& stream, Flat2DArray& data);

    ~Flat2DArray() noexcept;
};


template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray()
{
	_width = 0; _height = 0;
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray(size_t width, size_t height) noexcept :
	_width(width),
	_height(height)
{
	/*this->_width = width;
	this->_height = height;*/
	
	this->_object = make_unique<SharedMemoryObject>(this->_width * this->_height, sizeof(T));

	auto settings = _object->create();
	this->_array = static_cast<T*>(settings.array);
}

//template<typename T, typename Enabled>
//Flat2DArray<T, Enabled>::Flat2DArray(T* array, size_t width, size_t height) noexcept
//{
//	this->_width = width;
//	this->_height = height;
//	this->_array = array;
//}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray(const Flat2DArray<T, Enabled>& other)
{
	_width = other._width;
	_height = other._height;
	const size_t capacity = _width * _height;

	
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
T& Flat2DArray<T, Enabled>::at(size_t x)
{
	return _array[x];
}

template<typename T, typename Enabled>
T& Flat2DArray<T, Enabled>::at(size_t x, size_t y)
{
	return _array[y * _width + x];
}

template<typename T, typename Enabled>
T& Flat2DArray<T, Enabled>::operator[](size_t posX) noexcept
{
	return _array[posX];
}

template<typename T, typename Enabled>
T* Flat2DArray<T, Enabled>::data() const noexcept
{
	return _array;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::capacity() const
{
	return this->_width * this->_height;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::width() const
{
	return _width;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::height() const
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
