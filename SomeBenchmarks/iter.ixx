export module iter;

import std;
import test;

using std::string;
using std::array;


export class IterSum : public test<IterSum> {
public:
	IterSum() { name = "iter"; }

	inline const string getName_impl() const {
		return name;
	}

	inline array<uint8_t, 126> run_impl(array<uint8_t, 256>& object) const noexcept {
		array<uint8_t, 126> res{};
		
		for (int i = 0; i < 126; i++) {
			res[i] = object[i] + object[i + 2] + object[128 + i] + object[128 + i + 1] + object[128 + i + 2];
		}
		return res;
	}

	inline array<uint8_t, 70> run_impl(array<uint8_t, 112>& object) const noexcept {
		array<uint8_t, 70> res{};

		short i = 0;

		uint8_t before_row_i = 0,
			cur_row_i = 0,
			after_row_i = 0;

		array<uint8_t, 7> indexes{};

		for (short q = 0; q < 7; q++) {
			indexes[q] = q * 16;
		}


		for (size_t y = 1; y < 6; y++) {
			before_row_i = indexes[y - 1];
			cur_row_i = indexes[y];
			after_row_i = indexes[y + 1];

			for (size_t x = 1; x < 15; x++) {
				res[i] =
					object[x - 1 + before_row_i] + object[x + 0 + before_row_i] + object[x + 1 + before_row_i]	\
					+ object[x - 1 + cur_row_i] + object[x + 1 + cur_row_i]		\
					+ object[x - 1 + after_row_i] + object[x + 0 + after_row_i] + object[x + 1 + after_row_i];
				i++;
			}
		}

		return res;
	}


	template<typename T> requires allowed_type<T>
	Flat2DArray<T> run_horizontalSumImpl(Flat2DArray<T>& object) const noexcept {
		const size_t mid_index = object.width();
		const size_t res_width = mid_index - 2;


		auto res = Flat2DArray<T>(res_width, 1, false);

		T* ptr0 = nullptr;
		T* ptr1 = nullptr;

		for (int i = 0; i < res_width; i++) {
			ptr0 = res.data() + i;
			ptr1 = ptr0 + mid_index;
			res[i] = ptr0[0] + ptr0[2] + ptr1[0] + ptr1[1] + ptr1[2];
		}
		return res;
	}

	template<typename T> requires allowed_type<T>
	inline Flat2DArray<T> run_verticalSumImpl(Flat2DArray<T>& object) const noexcept {
		const size_t width = object.width();
		const size_t height = object.height();

		const size_t offset_from_zero = 0;

		auto res = Flat2DArray<T>(14, height - 2, false);

		size_t before_row_i = 0,
			cur_row_i = 0,
			after_row_i = 0;

		auto indexes = Flat2DArray<int>(height, 1, false);

		for (short q = 0; q < height; q++) {
			indexes[q] = offset_from_zero + q * width;
		}

		using std::array;

		T* data = object.data();

		T* ptr0 = nullptr;
		T* ptr1 = nullptr;
		T* ptr2 = nullptr;

		for (size_t y = 1, i = 0; y < height - 1; y++) {
			ptr0 = data + indexes[y - 1];
			ptr1 = data + indexes[y + 0];
			ptr2 = data + indexes[y + 1];


			for (size_t x = 0; x < 14; x++) {
				res[i] =
					ptr0[x] + ptr0[x + 1] + ptr0[x + 2] + \
					ptr1[x] +				ptr1[x + 2] + \
					ptr2[x] + ptr2[x + 1] + ptr2[x + 2];
				i++;
			}
		}
		return res;
	}
};
