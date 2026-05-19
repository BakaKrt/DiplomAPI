module;

export module bitsetBufferedRule;

import sumFilterBase;
import std;

using std::string;
using std::bitset;
using std::array;
using std::memcpy;

export class BitsetBufferedRule : public SumFilterBase<BitsetBufferedRule> {
private:
	bitset<9> ruleB;
	bitset<9> ruleS;
	static constexpr size_t windowSize = 8;
public:
	BitsetBufferedRule() {
		name = "bit buf";
		ruleB.set(2);
		ruleS.set(2).set(3);
	}

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	__declspec(noinline) void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		const size_t width = object.width();

		T* dataPtr = object.data();
		T* resPtr = to_save.data();

		for (size_t x = 0; x < width; x += windowSize) {
			array<uint8_t, windowSize> res {};

			for (size_t i = x; i < windowSize; i++) {
				T& saved = to_save[x];

				bool alive = object[x];

				bool b_contains = ruleB.test(saved);
				bool s_contains = ruleS.test(saved);

				res[i] = alive ? b_contains : s_contains;
			}

			memcpy(resPtr + x, res.data(), windowSize);
		}
	}
};