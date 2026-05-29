export module TotalOnlyScalarRealization;

import std;
import totalRealizationBase;

using std::string;
using std::array, std::bitset;
using std::memcpy;

export class OnlyScalarRealization : public TotalRealizationBase<OnlyScalarRealization> {
private:
	bitset<9> ruleB {};
	bitset<9> ruleS {};
public:
	OnlyScalarRealization() {
		name = "only scal";
		ruleB.set(3);
		ruleS.set(2).set(3);
	}

	inline const string getNameImpl() const {
		return name;
	}

	__declspec(noinline) void runImpl(Flat2DArray<uint8_t>& object, Flat2DArray<uint8_t>& to_save) const noexcept {
		using T = uint8_t;

		const size_t width = object.width();
		const size_t height = object.height();

		// временный массив для хранения временных сумм
		// в данной реализации от него никуда не деться
		T* temp_res_ptr = nullptr;
#ifdef _DEBUG
		auto temp_array_for_results = Flat2DArray<T>(width, 1, false);

		temp_res_ptr = temp_array_for_results.data();
#else
		T* temp_array_for_results = new T[width];
		temp_res_ptr = temp_array_for_results;
#endif // _DEBUG

		T* data_ptr = object.data();
		T* res_ptr = to_save.data();

		T* ptr0 = nullptr;
		T* ptr1 = nullptr;
		T* ptr2 = nullptr;

		auto saveRes = [&temp_res_ptr, &data_ptr, &res_ptr, this, width] (size_t save_offset) {
			constexpr size_t BLOCK_SIZE = 8;
			size_t full_blocks_end = (width / BLOCK_SIZE) * BLOCK_SIZE;

			for (size_t x = 0; x < full_blocks_end; x += BLOCK_SIZE) {
				size_t current_save_offset = save_offset + x;
				size_t current_temp_offset = x;

				alignas(8) T res_block[BLOCK_SIZE] {};

				for (size_t i = 0; i < BLOCK_SIZE; ++i) {
					T neighboursCount = temp_res_ptr[current_temp_offset + i];
					bool alive = data_ptr[current_save_offset + i];
					bool b_contains = ruleB.test(neighboursCount);
					bool s_contains = ruleS.test(neighboursCount);
					res_block[i] = alive ? b_contains : s_contains;
				}

				memcpy(res_ptr + current_save_offset, res_block, BLOCK_SIZE);
			}

			size_t remaining_elements = width - full_blocks_end;
			if (remaining_elements > 0) {
				size_t current_save_offset = save_offset + full_blocks_end;
				size_t current_temp_offset = full_blocks_end;

				alignas(8) T tail_block[BLOCK_SIZE] = {0};
				for (size_t i = 0; i < remaining_elements; ++i) {
					T neighboursCount = temp_res_ptr[current_temp_offset + i];
					bool alive = data_ptr[current_save_offset + i];
					bool b_contains = ruleB.test(neighboursCount);
					bool s_contains = ruleS.test(neighboursCount);
					tail_block[i] = static_cast<T>(alive ? b_contains : s_contains);
				}
				memcpy(res_ptr + current_save_offset, tail_block, remaining_elements);
			}
		};

#pragma region first_lines
		// обработка первых двух строк, то есть 
		// ptr0 = 0    1    2    3    4    5
		// ptr1 = 6    7    8    9   10   11
		ptr0 = data_ptr;
		ptr1 = data_ptr + width;
		for (size_t x = 0; x < width; x++) {
			// temp_res_ptr = 0 + 6, 1 + 7, 2 + 8 и так далее
			temp_res_ptr[x] = ptr0[x] + ptr1[x];
		}
		// верхний левый элемент = сумма (0 + 6) + (1 + 7) - 0
		res_ptr[0] = temp_res_ptr[0] + temp_res_ptr[1] - ptr0[0];

		// верхние элементы.
		// TO DO: Вообще можно попытаться избавиться от двойного цикла, попробовав
		// использовать верхний цикл вместого этого, но это надо подумать
		for (size_t x = 1; x < width - 1; x++) {
			res_ptr[x] = temp_res_ptr[x - 1] + temp_res_ptr[x] + temp_res_ptr[x + 1] - ptr0[x];
		}

		// верхний правый элемент = сумма (4 + 10) + (5 + 11) - 5
		res_ptr[width - 1] = temp_res_ptr[width - 2] + temp_res_ptr[width - 1] - ptr0[width - 1];
		saveRes(0);
#pragma endregion

#pragma region mid
		// обработка средних элементов
		for (size_t y = 0; y < height - 2; y++) {
			ptr0 = data_ptr + (width * y);
			ptr1 = data_ptr + (width * y + width);
			ptr2 = data_ptr + (width * y + 2 * width);

			const size_t offset_to_result_row = (width * y + width);

			// в результирующую строку помещаются поэлементные суммы трёх строк
			// ptr0 указывает на 0, ptr1 на 6, ptr2 на 12. В результирующую строку помещается сумма 0 + 6 + 12, затем 1 + 7 + 13 и так далее
			for (size_t x = 0; x < width; x++) {
				temp_res_ptr[x] = ptr0[x] + ptr1[x] + ptr2[x];
			}

			// крайний левый
			res_ptr[offset_to_result_row] = temp_array_for_results[0] + temp_array_for_results[1] - ptr1[0];

			// по середине
			for (size_t x = 1; x < width - 1; x++) {
				res_ptr[x + offset_to_result_row] =
					// сумма соседей слева + по середине + справа
					temp_res_ptr[x - 1] + temp_res_ptr[x] + temp_res_ptr[x + 1] \
					// центральный элемент не должен быть включён в сумму
					- ptr1[x];
			}

			// крайний правый
			res_ptr[offset_to_result_row + width - 1] = temp_res_ptr[width - 2] + temp_res_ptr[width - 1] - ptr1[width - 1];
			saveRes(offset_to_result_row);
		}
#pragma endregion

#pragma region last_row
		size_t last_row_offset = (height - 1) * width;
		// обработка последнего ряда
		for (size_t x = 0; x < width; x++) {
			// в ptr1 и ptr2 хранятся последние 2 ряда
			temp_res_ptr[x] = ptr1[x] + ptr2[x];
		}

		// нижний левый
		res_ptr[last_row_offset] = temp_res_ptr[0] + temp_res_ptr[1] - ptr2[0];

		// середина
		for (size_t x = 1; x < width - 1; x++) {
			res_ptr[last_row_offset + x] = temp_res_ptr[x - 1] + temp_res_ptr[x] + temp_res_ptr[x + 1] - ptr2[x];
		}

		// нижний правый
		res_ptr[last_row_offset + width - 1] = temp_res_ptr[width - 1] + temp_res_ptr[width - 2] - ptr2[width - 1];

		saveRes(last_row_offset);
#pragma endregion
#ifdef NDEBUG
		delete[] temp_array_for_results;
#endif // _NDEBUG
	}
};
