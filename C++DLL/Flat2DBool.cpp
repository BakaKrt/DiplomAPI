#include "Flat2DBool.h"



Flat2DBool::Flat2DBool() {
    Capacity = 0;
    Width = 0, Height = 0;
    Array = nullptr;
}

Flat2DBool::Flat2DBool(size_t width, size_t height) noexcept {
    this->Width = width;
    this->Height = height;
    this->Capacity = this->Width * this->Height;
    this->Array = new bool[Capacity]();
}

Flat2DBool::Flat2DBool(bool* array, size_t width, size_t height) noexcept
{
    this->Array = array;
    this->Capacity = width * height;
    this->Width = width;
    this->Height = height;
}

Flat2DBool::Flat2DBool(const Flat2DBool& other) {
    Width = other.Width;
    Height = other.Height;
    Capacity = other.Capacity;

    if (other.Array) {
        Array = new bool[Capacity];
        std::memcpy(Array, other.Array, Capacity);
    }
    else {
        Array = nullptr;
    }
}

Flat2DBool& Flat2DBool::operator=(const Flat2DBool& other) {
    if (this != &other) { // Проверка на самоприсваивание
        delete[] Array; // Освобождаем старую память

        Width = other.Width;
        Height = other.Height;
        Capacity = other.Capacity;

        if (other.Array) {
            Array = new bool[Capacity];
            std::memcpy(Array, other.Array, Capacity);
        }
        else {
            Array = nullptr;
        }
    }
    return *this;
}

bool& Flat2DBool::at(size_t x) {
    return Array[x];
}

bool& Flat2DBool::at(size_t x, size_t y) {
    return Array[y * Width + x];
}

bool& Flat2DBool::operator[] (size_t posX) noexcept {
    return Array[posX];
}

bool* Flat2DBool::data() const noexcept {
    return Array;
}
size_t Flat2DBool::capacity() const { return this->Capacity; }
size_t Flat2DBool::width()    const { return this->Width; }
size_t Flat2DBool::height()   const { return this->Height; }

std::ostream& operator <<(std::ostream& stream, Flat2DBool& data) {
    for (size_t i = 0; i < data.Capacity;)
    {
        stream << (int)data.at(i) << "\t";
        i++;
        if (i % data.Width == 0 && i != 0) stream << '\n';
        else stream << ' ';
    }
    return stream;
}

Flat2DBool::~Flat2DBool() noexcept {
    delete[] Array;
}