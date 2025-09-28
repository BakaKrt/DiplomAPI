#include "Flat2DByte.h"



Flat2DByte::Flat2DByte() {
    Capacity = 0;
    Width = 0, Height = 0;
    Array = nullptr;
}

Flat2DByte::Flat2DByte(size_t Width, size_t Height) noexcept {
    this->Width = Width;
    this->Height = Height;
    this->Capacity = this->Width * this->Height;
    this->Array = new byte[Capacity]();
}

Flat2DByte::Flat2DByte(const Flat2DByte& other) {
    Width = other.Width;
    Height = other.Height;
    Capacity = other.Capacity;

    if (other.Array) {
        Array = new byte[Capacity];
        for (size_t i = 0; i < Capacity; ++i) {
            Array[i] = other.Array[i];
        }
    }
    else {
        Array = nullptr;
    }
}

Flat2DByte& Flat2DByte::operator=(const Flat2DByte& other) {
    if (this != &other) { // Проверка на самоприсваивание
        delete[] Array; // Освобождаем старую память

        Width = other.Width;
        Height = other.Height;
        Capacity = other.Capacity;

        if (other.Array) {
            Array = new byte[Capacity];
            for (size_t i = 0; i < Capacity; ++i) {
                Array[i] = other.Array[i];
            }
        }
        else {
            Array = nullptr;
        }
    }
    return *this;
}

byte& Flat2DByte::at(size_t x) {
//#ifdef _DEBUG
//    if (x >= Capacity) {
//        throw std::out_of_range("Index out of range");
//    }
//#endif // DEBUG
    return Array[x];
}

byte& Flat2DByte::at(size_t x, size_t y) {
//#ifdef _DEBUG
//    if (x >= Width || y >= Height) {
//        throw std::out_of_range("Index out of range");
//    }
//#endif // DEBUG
    return Array[y * Width + x];
}

byte& Flat2DByte::operator[] (size_t posX) noexcept {
    return Array[posX];
}

byte* Flat2DByte::data() const noexcept {
    return Array;
}
size_t Flat2DByte::capacity() const { return this->Capacity; }
size_t Flat2DByte::width()    const { return this->Width; }
size_t Flat2DByte::height()   const { return this->Height; }

std::ostream & operator <<(std::ostream & stream, Flat2DByte & data) {
    for (size_t i = 0; i < data.Capacity;)
    {
        stream << (int)data.at(i) << "\t";
        i++;
        if (i % data.Width == 0 && i != 0) stream << '\n';
        else stream << ' ';
    }
    return stream;
}

Flat2DByte::~Flat2DByte() noexcept {
    delete[] Array;
}