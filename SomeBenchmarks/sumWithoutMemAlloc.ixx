export module normalsumv2;

import std;
import sumRealizationBase;

using std::string;
using std::array;

export class NormalWOAllocSum : public SumRealizationBase<NormalWOAllocSum> {
public:
	NormalWOAllocSum() { name = "v lob"; }

	inline const string getName_impl() const {
		return name;
	}


	template<typename T> requires allowed_type<T>
	__declspec(noinline) void test_runImpl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {

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

		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				sum = 0;
				if (x > 0 && x + 1 < width && y > 0 && y + 1 < height) {
					sum += object.at(x - 1, y - 1); // верхний левый
					sum += object.at(x	  , y - 1);	// верхний
					sum += object.at(x + 1, y - 1); // верхний правый

					sum += object.at(x - 1, y	 ); // левый
					sum += object.at(x + 1, y	 ); // правый

					sum += object.at(x - 1, y + 1); // нижний левый
					sum += object.at(x	  , y + 1);	// нижний
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

				to_save.at(x, y) = sum;
			}
		}
	}
};
