export module test;

import std;

using std::string;
using std::array;


export class test {
protected:
	string name{};
public:
	test() = default;
	string getName() { return name; }

	virtual float run(array<float, 3>& object) const noexcept = 0;
	virtual float run(array<float, 5>& object) const noexcept = 0;
	virtual float run(array<float, 8>& object) const noexcept = 0;

	/// <summary>
	/// Сумма для поиска суммы соседей в углу
	/// </summary>
	/// <param name="object"></param>
	/// <returns></returns>
	virtual array<float, 4> run(array<float, 12>& object) const noexcept = 0;

	/// <summary>
	/// Сумма для массива:
	///  0 1 2  9 10
	///  3 4 5 11 12
	///  6 7 8 13 14
	/// </summary>
	/// <param name="object"></param>
	/// <returns>Сумма для соседей под номерами 4, 5 и 11</returns>
	virtual array<float, 3> run(array<float, 15>& object) const noexcept = 0;

	/// <summary>
	/// Сумма для массива:
	/// 0  1   2  9 10 11
	/// 3  4   5 12 13 14
	/// 6  7   8 15 16 17
	/// 18 19 20 21 22 23
	/// </summary>
	/// <param name="object"></param>
	/// <returns>Сумма для соседей под номерами: 4, 5 12, 13, 7, 8, 15, 16</returns>
	virtual array<float, 8> run(array<float, 24>& object) const noexcept = 0;

	virtual array<uint8_t, 4> run(array<uint8_t, 12>& object) const noexcept = 0;

	virtual array<uint8_t, 126> run(array<uint8_t, 256>& object) const noexcept = 0;

	virtual array<uint8_t, 70> run(array<uint8_t, 112>& object) const noexcept = 0;

	virtual ~test() = default;
};
