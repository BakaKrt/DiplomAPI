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
		ruleB.set(3);
		ruleS.set(2).set(3);
	}

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	__declspec(noinline) void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		const size_t object_capacity = object.width() * object.height();

		T* dataPtr = object.data();
		T* resPtr = to_save.data();

		array<uint8_t, windowSize> res {};

		size_t x = 0;
		for (; x < object_capacity - windowSize; x += windowSize) {
			for (size_t i = 0; i < windowSize; i++) {
				T& neighbours_count = resPtr[x + i];

				bool alive = dataPtr[x + i];

				bool s_contains = ruleS.test(neighbours_count);
				bool b_contains = ruleB.test(neighbours_count);

				res[i] = alive ? s_contains : b_contains;
			}

			memcpy(resPtr + x, res.data(), windowSize);
		}
		size_t remainder = object_capacity % windowSize;
		if (remainder != 0) {
			for (; x < object_capacity; x++) {
				T& neighbours_count = resPtr[x];

				bool alive = dataPtr[x];

				bool s_contains = ruleS.test(neighbours_count);
				bool b_contains = ruleB.test(neighbours_count);

				neighbours_count = alive ? s_contains : b_contains;
			}
		}
	}
};