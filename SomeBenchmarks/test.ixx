export module test;

import std;
import Flat2DArray;

using std::string;
using std::array;



export 
template<typename Derived>
class test {
protected:
    string name {};

public:
    test() = default;

    inline const string getName() const {
        return static_cast<const Derived*>(this)->getName_impl();
    }

    inline array<uint8_t, 126> run(array<uint8_t, 256>& object) const noexcept {
        return static_cast<const Derived*>(this)->run_impl(object);
    }

    inline array<uint8_t, 70> run(array<uint8_t, 112>& object) const noexcept {
        return static_cast<const Derived*>(this)->run_impl(object);
    }

    template<typename T> requires allowed_type<T>
    inline Flat2DArray<T> runHorizontalSum(Flat2DArray<T>& object) const noexcept {
        return static_cast<const Derived*>(this)->run_horizontalSumImpl(object);
    }

    template<typename T> requires allowed_type<T>
    inline Flat2DArray<T> runVerticalSum(Flat2DArray<T>& object) const noexcept {
        return static_cast<const Derived*>(this)->run_verticalSumImpl(object);
    }
    
    template<typename T> requires allowed_type<T>
    inline Flat2DArray<T> runHorizontalNextLineSum(Flat2DArray<T>& object) const noexcept {
        return static_cast<const Derived*>(this)->run_horizontalNextLineSum(object);
    }

    virtual ~test() = default; // Если нужно, можно оставить
};