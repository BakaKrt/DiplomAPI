export module normalRule;

import sumFilterBase;
import std;

using std::string;


export class NormalRule: public SumFilterBase<NormalRule> {
private:
	std::unordered_set<int> ruleB;
	std::unordered_set<int> ruleS;
public:
	NormalRule() {
		name = "normal";
		ruleB = { 3 };
		ruleS = { 2, 3 };
	}

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	__declspec(noinline) void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& neighbours) const noexcept {
		const size_t object_capacity = object.width() * object.height();

		for (size_t x = 0; x < object_capacity; x++) {
			T& neighbours_count = neighbours[x];

			bool alive = object[x];

			if (alive && ruleS.contains(neighbours_count)) {
				neighbours_count = true;
			}
			else if (!alive && ruleB.contains(neighbours_count)) {
				neighbours_count = true;
			}
			else {
				neighbours_count = false;
			}
		}
	}
};