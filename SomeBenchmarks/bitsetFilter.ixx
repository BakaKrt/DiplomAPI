module;

export module bitsetRule;

import sumFilterBase;
import std;

using std::string;
using std::bitset;

export class BitsetRule : public SumFilterBase<BitsetRule> {
private:
	bitset<9> ruleB;
	bitset<9> ruleS;
public:
	BitsetRule() {
		name = "bitset";
		ruleB.set(2);
		ruleS.set(2).set(3);
	}

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	__declspec(noinline) void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		const size_t width = object.width();

		for (size_t x = 0; x < width; x++) {
			T& saved = to_save[x];

			bool alive = object[x];

			bool b_contains = ruleB.test(saved);
			bool s_contains = ruleS.test(saved);

			saved = alive ? b_contains : s_contains;
		}
	}
};