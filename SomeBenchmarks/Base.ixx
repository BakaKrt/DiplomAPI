export module totalRealizationBase;

import std;
import Flat2DArray;

using std::string;
using std::array;



export
template<typename Derived>
class TotalRealizationBase {
protected:
    string name {};
public:
    inline const string getName() const {
        return static_cast<const Derived*>(this)->getNameImpl();
    }

    template<typename T> requires allowed_type<T>
    inline Flat2DArray<T> run(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
        static_cast<const Derived*>(this)->runImpl(object, to_save);
        return to_save;
    }
};