#include <cmath>

export module ModuledMatrix;

import std;
import CPUInfo;
import FlatArray;

using namespace CpuHelper;

using std::vector;
using std::unique_ptr;
using std::make_unique;


export namespace ModuledMatrix {
	export template<typename T> requires is_allowed_type<T>::value
	class Matrix {
	private:
		int columns;
		int rows;

		size_t size_x;
		size_t size_y;

		vector<unique_ptr<Flat2DArray<T>>> arrays;
		CpuInfo info;
		size_t block_size;
	public:
		Matrix(size_t size_x, size_t size_y) noexcept: size_x(size_x), size_y(size_y)
		{

		}

		bool Init() {
			if (info.Init() == false) {
				return false;
			}

			size_t elements_count_in_L1_cache = info.L1_size / sizeof(T);

			const size_t CACHE_LINE_SIZE = 64; // размер X

			size_t elements_in_cache_line = CACHE_LINE_SIZE / sizeof(T);

			size_t size_y = elementsCountInL1cache / CACHE_LINE_SIZE;



			size_t totalSize = size_x * sizeof(T) * size_y;

			size_t full_part = totalSize / info.L1_size;
			size_t division_part = (bool)ceil(totalSize % info.L1_size);
			size_t matrix_parts_count = full_part + division_part;

			block_size = totalSize / matrix_parts_count;
			arrays.reserve(matrix_parts_count);

			// 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

			/*for (size_t x = 0; x < MatrixPartsCount; x++) {
				arrays.push_back(make_unique<Flat2DArray<T>>());
			}*/
			
			return true;
		}

		static inline bool IsPowerOfTwo(int n) {
			return (n & (n - 1)) == 0;
		}
	};
}