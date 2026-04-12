export module normalsum;

import std;
import test;

using std::string;
using std::array;

export class NormalSum : public test {
public:
	NormalSum() { name = "normal"; }

	inline float run(array<float, 3>& object) const noexcept override {
		float res = 0;
		res += object[0] + object[1] + object[2];
		return res;
	}

	inline float run(array<float, 5>& object) const noexcept override {
		float res = 0;
		res += res += object[0] + object[1] + object[2] + \
			object[3] + object[4];
		return res;
	}

	inline float run(array<float, 8>& object) const noexcept override {
		float res = 0;
		res += object[0] + object[1] + object[2] + object[3] + \
			object[4] + object[5] + object[6] + object[7];
		return res;
	}

	inline array<float, 3> run(array<float, 15>& object) const noexcept override {
		array<float, 3> res{};

		res[0] = object[0] + object[1] + object[2] + object[3] + \
			object[5] + object[6] + object[7] + object[8];

		res[1] = object[1] + object[2] + object[4] + object[7] + \
			object[11] + object[8] + object[9] + object[13];

		res[2] = object[2] + object[5] + object[8] + object[9] + \
			object[10] + object[12] + object[13] + object[14];

		return res;
	}

	inline array<float, 4> run(array<float, 12>& object) const noexcept override {
		array<float, 4> res{};

		res[0] = object[0] + object[1] + object[2];

		res[1] = object[3] + object[4] + object[5];

		res[2] = object[6] + object[7] + object[8];

		res[3] = object[9] + object[10] + object[11];

		return res;
	}

	/// 0  1   2  9 10 11
	/// 3  4   5 12 13 14
	/// 6  7   8 15 16 17
	/// 18 19 20 21 22 23
	inline array<float, 8> run(array<float, 24>& object) const noexcept override {
		array<float, 8> res{};

		res[0] = object[0] + object[1] + object[2] + object[3] + \
			object[5] + object[6] + object[7] + object[8];		// 4

		res[1] = object[1] + object[2] + object[4] + object[7] + \
			object[9] + object[8] + object[12] + object[15];	// 5

		res[2] = object[2] + object[5] + object[8] + object[9] + \
			object[10] + object[13] + object[15] + object[16];	// 12

		res[3] = object[9] + object[10] + object[11] + object[12] + \
			object[14] + object[15] + object[16] + object[17];	// 13


		res[4] = object[3] + object[4] + object[5] + object[6] + \
			object[8] + object[18] + object[19] + object[20];	// 7

		res[5] = object[4] + object[5] + object[7] + object[12] + \
			object[15] + object[19] + object[20] + object[21];	// 8

		res[6] = object[5] + object[8] + object[12] + object[13] + \
			object[16] + object[20] + object[21] + object[22];	// 15

		res[7] = object[12] + object[13] + object[14] + object[15] + \
			object[20] + object[21] + object[22] + object[23];	// 16
		return res;
	}

	inline array<uint8_t, 4> run(array<uint8_t, 12>& object) const noexcept override {
		array<uint8_t, 4> res{};

		res[0] = object[0] + object[1] + object[2];
		res[1] = object[3] + object[4] + object[5];
		res[2] = object[6] + object[7] + object[8];
		res[3] = object[9] + object[10] + object[11];

		return res;
	}

	inline array<uint8_t, 126> run(array<uint8_t, 256>& object) const noexcept override {
		array<uint8_t, 126> res{};

		for (int i = 0; i < 126; i++) {
			res[i] = object[i] + object[i + 2] + object[128 + i] + object[128 + i + 1] + object[128 + i + 2];
		}
		return res;
	}

	inline array<uint8_t, 70> run(array<uint8_t, 112>& object) const noexcept override {
		array<uint8_t, 70> res{};

		//int sum = 0;
		//int row = 0;

		//int neigh_sum = 0;

		//for (int y = 0; y < 7; y++) {
		//	for (int x = 0; x < 14; x+=2) {
		//		row = y * 16;

		//		neigh_sum = object[x + 1 + row] + object[x + 2 + row];

		//		sum = object[x + row];
		//	}
		//}


		short i = 0;

		uint8_t before_row_i = 0,
		cur_row_i	= 0,
		after_row_i = 0;

		array<uint8_t, 7> indexes{};

		for (short q = 0; q < 7; q++) {
			indexes[q] = q * 16;
		}


		for (size_t y = 1; y < 6; y++) {
			before_row_i = indexes[y - 1];
			cur_row_i	 = indexes[y];
			after_row_i  = indexes[y + 1];

			for (size_t x = 1; x < 15; x++) {
				res[i] = 
					  object[x - 1 + before_row_i]	+ object[x + 0 + before_row_i]	+ object[x + 1 + before_row_i]	\
					+ object[x - 1 + cur_row_i]										+ object[x + 1 + cur_row_i]		\
					+ object[x - 1 + after_row_i]	+ object[x + 0 + after_row_i]	+ object[x + 1 + after_row_i];
				i++;
			}
		}

		return res;
	}
};
