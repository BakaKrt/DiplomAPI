export module normalOptimizedRule;

import sumFilterBase;
import std;

using std::string;


export class NormalRuleIfOpt : public SumFilterBase<NormalRuleIfOpt> {
private:
	std::unordered_set<int> ruleB;
	std::unordered_set<int> ruleS;
public:
	NormalRuleIfOpt() {
		name = "norm o";
		ruleB = { 2 };
		ruleS = { 2, 3 };
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

			bool b_contains = ruleB.contains(saved);
			bool s_contains = ruleS.contains(saved);

			saved = alive ? b_contains : s_contains;
		}
	}
};