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

    template<typename T> requires allowed_type<T>
    inline Flat2DArray<T> test_run(Flat2DArray<T>& object) const noexcept {
		auto temp_array = Flat2DArray<T>(object.width(), object.height(), false);
        static_cast<const Derived*>(this)->test_runImpl(object, temp_array);

        return temp_array;
    }

    ~test() = default; // Если нужно, можно оставить
};