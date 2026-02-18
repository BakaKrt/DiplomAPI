#pragma once

#include <cstdint>

import std;

using std::ostream;

using byte = std::uint8_t;

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
    size_t Capacity;
    size_t Width, Height;
    T* Array;
    friend class HeightMap;
public:
    Flat2DArray();

    Flat2DArray(size_t width, size_t height) noexcept;

    Flat2DArray(T* array, size_t width, size_t height) noexcept;

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
	Capacity = 0;
	Width = 0; Height = 0;
	Array = nullptr;
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray(size_t width, size_t height) noexcept
{
	this->Width = width;
	this->Height = height;
	this->Capacity = width * height;
	this->Array = new T[Capacity]();
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray(T* array, size_t width, size_t height) noexcept
{
	this->Width = width;
	this->Height = height;
	this->Capacity = width * height;
	this->Array = array;
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::Flat2DArray(const Flat2DArray& other)
{
	Width = other.Width;
	Height = other.Height;
	Capacity = other.Capacity;

	if (other.Array) {
		Array = new T[Capacity];
		std::memcpy(Array, other.Array, Capacity);
	}
	else {
		Array = nullptr;
	}
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>& Flat2DArray<T, Enabled>::operator=(const Flat2DArray& other)
{
	if (this != &other) { // Проверка на самоприсваивание
		delete[] Array; // Освобождаем старую память

		Width = other.Width;
		Height = other.Height;
		Capacity = other.Capacity;

		if (other.Array) {
			Array = new T[Capacity];
			std::memcpy(Array, other.Array, Capacity);
		}
		else {
			Array = nullptr;
		}
	}
	return *this;
}

template<typename T, typename Enabled>
T& Flat2DArray<T, Enabled>::at(size_t x)
{
	return Array[x];
}

template<typename T, typename Enabled>
T& Flat2DArray<T, Enabled>::at(size_t x, size_t y)
{
	return Array[y * Width + x];
}

template<typename T, typename Enabled>
T& Flat2DArray<T, Enabled>::operator[](size_t posX) noexcept
{
	return Array[posX];
}

template<typename T, typename Enabled>
T* Flat2DArray<T, Enabled>::data() const noexcept
{
	return Array;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::capacity() const
{
	return Capacity;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::width() const
{
	return Width;
}

template<typename T, typename Enabled>
size_t Flat2DArray<T, Enabled>::height() const
{
	return Height;
}

template<typename T, typename Enabled>
Flat2DArray<T, Enabled>::~Flat2DArray() noexcept
{
	delete[] Array;
	Array = nullptr;
	Capacity = 0;
	Width = 0; Height = 0;
}

template<typename T, typename Enabled>
ostream& operator<<(ostream& stream, Flat2DArray<T, Enabled>& data)
{
	for (size_t i = 0; i < data.Capacity;)
	{
		stream << (int)data.at(i) << "\t";
		i++;
		if (i % data.Width == 0 && i != 0) stream << '\n';
		else stream << ' ';
	}
	return stream;
}
