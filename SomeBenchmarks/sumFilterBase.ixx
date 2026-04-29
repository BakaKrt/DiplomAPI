export module sumFilterBase;

import std;
import Flat2DArray;

using std::string;

/// <summary>
/// Базовый класс. Для сравнения скорости выполнения применения правил игры жизнь (B = 3; S = 2, 3) на исходный массив А, к количеству соседей элементов массива А в массиве B
/// </summary>
export 
template<typename Derived>
class SumFilterBase {
protected:
	string name {};
public:
	inline const string getName() const {
		return static_cast<const Derived*>(this)->getName_impl();
	}

	template<typename T> requires allowed_type<T>
	inline void applyRule(Flat2DArray<T>& object, Flat2DArray<T>& sums) const noexcept {
		static_cast<const Derived*>(this)->applyRule_impl(object, sums);
		return;
	}
};