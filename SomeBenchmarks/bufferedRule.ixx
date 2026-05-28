export module bufferedRule;

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
		ruleB = { 3 };
		ruleS = { 2, 3 };
	}

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	__declspec(noinline) void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& neighbours) const noexcept {
		const size_t object_capacity = object.width() * object.height();

		T* dataPtr = object.data();
		T* neighPtr = neighbours.data();

		array<uint8_t, windowSize> res {};
		
		size_t x = 0;
		for (; x < object_capacity - windowSize; x += windowSize) {
			for (size_t i = 0; i < windowSize; i++) {
				T& neighbours_count = neighPtr[x + i];

				bool alive = dataPtr[x + i];

				bool b_contains = ruleB.contains(neighbours_count);
				bool s_contains = ruleS.contains(neighbours_count);

				res[i] = alive ? s_contains : b_contains;
			}
			memcpy(neighPtr + x, res.data(), windowSize);
		}

		size_t remainder = object_capacity % windowSize;
		if (remainder != 0) {
			for (; x < object_capacity; x++) {
				T& neighbours_count = neighPtr[x];

				bool alive = dataPtr[x];

				bool b_contains = ruleB.contains(neighbours_count);
				bool s_contains = ruleS.contains(neighbours_count);

				neighbours_count = alive ? b_contains : s_contains;
			}
		}
	}
};