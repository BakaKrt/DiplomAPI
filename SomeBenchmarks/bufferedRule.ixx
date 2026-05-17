export module BufferedRule;

import sumFilterBase;
import std;

using std::string;
using std::array;

using std::memcpy;

export class BufferedRule : public SumFilterBase<BufferedRule> {
private:
	std::unordered_set<int> ruleB;
	std::unordered_set<int> ruleS;

	static constexpr size_t windowSize = 8;
public:
	BufferedRule() {
		name = "buffered";
		ruleB = { 2 };
		ruleS = { 2, 3 };
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
			static array<uint8_t, windowSize> res {};

			for (size_t i = x; i < windowSize; i++) {
				T& saved = to_save[x];

				bool alive = object[x];

				bool b_contains = ruleB.contains(saved);
				bool s_contains = ruleS.contains(saved);

				res [i] = alive ? b_contains : s_contains;
			}

			memcpy(resPtr + x, res.data(), windowSize);
		}
	}
};