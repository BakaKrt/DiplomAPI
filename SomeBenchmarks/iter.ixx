export module iter;

import std;
import test;

using std::string;
using std::array;


export class IterSum : public test {
public:
	IterSum() { name = "iter"; }

	inline float run(array<float, 3>& object) const noexcept override {
		float res = 0;
		for (const float& a : object) res += a;
		return res;
	}

	inline float run(array<float, 5>& object) const noexcept override {
		float res = 0;
		for (const float& a : object) res += a;
		return res;
	}

	inline float run(array<float, 8>& object) const noexcept override {
		float res = 0;
		for (const float& a : object) res += a;
		return res;
	}

	inline array<float, 4> run(array<float, 12>& object) const noexcept override {
		array<float, 4> res{};
		for (int x = 0; x < 3; x++) res[0] += object[x];
		for (int x = 3; x < 6; x++) res[1] += object[x];
		for (int x = 6; x < 9; x++) res[2] += object[x];
		for (int x = 9; x < 12; x++) res[3] += object[x];

		return res;
	}

	inline array<float, 3> run(array<float, 15>& object) const noexcept override {
		array<float, 3> res{ 0.0f, 0.0f, 0.0f };
		for (int x = 0; x < 9; x++) res[0] += object[x];

		res[1] = object[1] + object[2] + object[9] + object[4] + \
			object[11] + object[7] + object[8] + object[13];

		res[2] = object[2] + object[9] + object[10] + object[5] + \
			object[12] + object[8] + object[13] + object[14];

		return res;
	}

	/// 0  1   2  9 10 11
	/// 3  4   5 12 13 14
	/// 6  7   8 15 16 17
	/// 18 19 20 21 22 23
	inline array<float, 8> run(array<float, 24>& object) const noexcept override {
		array<float, 8> res{};
		for (int x = 0; x <= 8; x++) res[0] += object[x];
		res[0] -= object[4];

		for (int x = 9; x <= 17; x++) res[3] += object[x];
		res[3] -= object[13];

		res[1] = object[1] + object[2] + object[4] + object[7] + \
			object[9] + object[8] + object[12] + object[15];	// 5

		res[2] = object[2] + object[5] + object[8] + object[9] + \
			object[10] + object[13] + object[15] + object[16];	// 12


		res[4] = object[3] + object[4] + object[5] + object[6] + \
			object[8] + object[18] + object[19] + object[20];	// 7

		res[5] = object[4] + object[5] + object[7] + object[12] + \
			object[15] + object[19] + object[20] + object[21];	// 8

		res[6] = object[5] + object[8] + object[12] + object[13] + \
			object[16] + object[20] + object[21] + object[22];	// 15

		res[7] = object[12] + object[13] + object[14] + object[15] + \
			object[17] + object[21] + object[22] + object[23];	// 16

		return res;
	}

	inline array<uint8_t, 4> run(array<uint8_t, 12>& object) const noexcept override {
		array<uint8_t, 4> res{};

		for (int x = 0; x < 3; x++) res[0] += object[x];
		for (int x = 3; x < 6; x++) res[1] += object[x];
		for (int x = 6; x < 9; x++) res[2] += object[x];
		for (int x = 9; x < 12; x++) res[3] += object[x];

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

		short i = 0;

		uint8_t before_row_i = 0;
		uint8_t cur_row_i = 0;
		uint8_t after_row_i = 0;

		for (size_t y = 1; y < 6; y++) {
			before_row_i = (y - 1) * 16;
			cur_row_i = (y + 0) * 16;
			after_row_i = (y + 1) * 16;

			for (size_t x = 1; x < 14; x++) {
				res[i] =
					object[x - 1 + before_row_i] + object[x + 0 + before_row_i] + object[x + 1 + before_row_i]	\
					+ object[x - 1 + cur_row_i] + object[x + 1 + cur_row_i]		\
					+ object[x - 1 + after_row_i] + object[x + 0 + after_row_i] + object[x + 1 + after_row_i];
				i++;
			}
		}

		return res;
	}
};
