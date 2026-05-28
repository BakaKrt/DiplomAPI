export module TotalNaiveRealization;

import std;
import totalRealizationBase;

using std::string;
using std::array, std::unordered_set;

export class NaiveRealization : public TotalRealizationBase<NaiveRealization> {
private:
	unordered_set<int> ruleB;
	unordered_set<int> ruleS;
public:
	NaiveRealization() {
		name = "naive"; 	
		ruleB = { 3 };
		ruleS = { 2, 3 };
	}

	inline const string getNameImpl() const {
		return name;
	}

	__declspec(noinline) void runImpl(Flat2DArray<uint8_t>& object, Flat2DArray<uint8_t>& to_save) const noexcept {
		using T = uint8_t;
#define _DEBUG_SSE_HORIZONTAL 1
#if defined(_DEBUG) && _DEBUG_SSE_HORIZONTAL == 1
		auto DEBUG_RES = [&to_save] (string at_moment) {
			std::cout << "to_save " << at_moment << "\n" << to_save << "\n";
			};
#else
#define DEBUG_RES(at_moment) ((void)0)
#endif // _DEBUG

		const size_t width = object.width();
		const size_t height = object.height();

		T sum = 0;
		size_t sum_place_iterator = 0;
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				sum = 0;
				if (x > 0 && x + 1 < width && y > 0 && y + 1 < height) {
					sum += object.at(x - 1, y - 1); // верхний левый
					sum += object.at(x, y - 1);	// верхний
					sum += object.at(x + 1, y - 1); // верхний правый

					sum += object.at(x - 1, y); // левый
					sum += object.at(x + 1, y); // правый

					sum += object.at(x - 1, y + 1); // нижний левый
					sum += object.at(x, y + 1);	// нижний
					sum += object.at(x + 1, y + 1); // нижний правый
				}
				else {
					if (x > 0) {
						sum += object.at(x - 1, y);
						if (y > 0) sum += object.at(x - 1, y - 1);
						if (y + 1 < height) sum += object.at(x - 1, y + 1);
					}

					if (y > 0) sum += object.at(x, y - 1);
					if (y + 1 < height) sum += object.at(x, y + 1);

					if (x + 1 < width) {
						sum += object.at(x + 1, y);
						if (y > 0) sum += object.at(x + 1, y - 1);
						if (y + 1 < height) sum += object.at(x + 1, y + 1);
					}
				}

				sum_place_iterator = y * width + x;

				bool alive = object[sum_place_iterator];

				bool value = false;

				if (alive && ruleS.contains(sum)) {
					value = true;
				}
				else if (!alive && ruleB.contains(sum)) {
					value = true;
				}
				else {
					value = false;
				}

				to_save[sum_place_iterator] = value;
			}
		}
	}
};
