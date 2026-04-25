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

	template<typename T> requires allowed_type<T>
	inline void test_runImpl(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		run_SumAll(object, to_save);
	}

	template<typename T> requires allowed_type<T>
	inline Flat2DArray<T> run_horizontalSumImpl(Flat2DArray<T>& object) const noexcept {
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

		//auto indexes = Flat2DArray<int>(height, 1, false);
		size_t* indexes = new size_t[height];


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
		delete indexes;

		return res;
	}

	template<typename T> requires allowed_type<T>
	inline Flat2DArray<T> run_horizontalNextLineSum(Flat2DArray<T>& object) const noexcept {
		return object;
	}

	template<typename T> requires allowed_type<T>
	inline void run_SumAll(Flat2DArray<T>& object, Flat2DArray<T>& to_save) const noexcept {
		// пример массива object
		//  0    1    2    3    4    5
		//  6    7    8    9   10   11
		// 12   13   14   15   16   17
		// 18   19   20   21   22   23
		// 24   25   26   27   28   29

		// для заданного массива object, должен получиться to_save:
		// 14   23   28   33   38   25 - ок
		// 33   56   64   72   80   52 - ок
		// 63  104  112  120  125   82 - ок
		// 93  152  160  168  176  112 - ок
		// 62  107  112  117  122   73 - ок

		const size_t width = object.width();
		const size_t height = object.height();

		T* temp_res_ptr = nullptr;
#ifdef _DEBUG
		auto offsets = Flat2DArray<int>(height, 1, false);
		auto temp_array_for_results = Flat2DArray<T>(width, 1, false);

		temp_res_ptr = temp_array_for_results.data();
#else
		size_t* offsets = new size_t[height];
		T* temp_array_for_results = new T[width];
		temp_res_ptr = temp_array_for_results;
#endif // _DEBUG

		// инициализирую массив смещений до нулевого элемента каждой строки, т.к. арифметика указателей очень быстрая
		for (short q = 0; q < height; q++) {
			offsets[q] = q * width;
		}

		T* data_ptr = object.data();
		T* res_ptr = to_save.data();

		T* ptr0 = nullptr;
		T* ptr1 = nullptr;
		T* ptr2 = nullptr;

		// обработка первых двух строк, то есть 
		// ptr0 = 0    1    2    3    4    5
		// ptr1 = 6    7    8    9   10   11
		ptr0 = data_ptr + offsets[0];
		ptr1 = data_ptr + offsets[1];
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


		// обработка средних элементов
		for (size_t y = 0; y < height - 2; y++) {
			ptr0 = data_ptr + offsets[y + 0];
			ptr1 = data_ptr + offsets[y + 1];
			ptr2 = data_ptr + offsets[y + 2];

			const size_t offset_to_result_row = offsets[y + 1];

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
		}

#pragma region last_row
		size_t last_row_offset = offsets[height - 1];
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
#pragma endregion
#ifdef NDEBUG
		delete[] offsets;
		delete[] temp_array_for_results;
#endif // _NDEBUG
	}
};
