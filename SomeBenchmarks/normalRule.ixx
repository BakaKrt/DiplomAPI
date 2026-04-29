export module normalRule;

import sumFilterBase;
import std;

using std::string;


export class NormalRule: public SumFilterBase<NormalRule> {
public:
	NormalRule() { name = "normal"; }

	inline const string getName_impl() const {
		return name;
	}

	template<typename T> requires allowed_type<T>
	__declspec(noinline) void applyRule_impl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		const size_t width = object.width();

		for (size_t x = 0; x < width; x++) {
			T& saved = to_save[x];

			bool alive = object[x];

			if (alive && (saved == 2 || saved == 3)) {
				saved = true;
			}
			else if (!alive && saved == 3) {
				saved = true;
			}
			else {
				saved = false;
			}
		}
	}
};