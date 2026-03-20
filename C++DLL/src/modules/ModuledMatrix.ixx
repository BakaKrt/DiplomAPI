#include <cmath>

export module ModuledMatrix;

import std;
import CPUInfo;
import FlatArray;

using namespace CpuHelper;

using std::vector;
using std::unique_ptr;
using std::make_unique;

using std::span;

export namespace ModuledMatrix {
	export template<typename T> requires is_allowed_type<T>::value
	class TileView {
	private:
		CpuInfo info;

		size_t _width;
		size_t _height;

		span<T> _data;
	public:
		TileView(Flat2DArray<T> array, size_t xs, size_t ys, size_t xe, size_t ye) noexcept: _width(xe-xs), _height(ye-ys)
		{
			//auto temp = span<T>(array.data(), array.capacity());
			//temp.;
		}

		bool Init() {
			if (info.Init() == false) {
				return false;
			}
			std::printf("L1 Data cache size: %u\n", info.L1_size);
			// 48kb = 49 152

			//info.L1_size = 49152;

			const size_t ELEMENTS_COUNT_IN_L1_CACHE = info.L1_size / sizeof(T);

			const size_t CACHE_LINE_SIZE_IN_BYTES = 64;
			const size_t ELEMENTS_COUNT_IN_CACHE_LINE = CACHE_LINE_SIZE_IN_BYTES / sizeof(T);
			
			//const size_t HEIGHT = ELEMENTS_COUNT_IN_L1_CACHE / size_x;
			//const size_t WIDTH = ELEMENTS_COUNT_IN_L1_CACHE / HEIGHT;
			
			//this->rows = this->size_x / HEIGHT;
			//this->columns = this->size_y / WIDTH;

			//const size_t TOTAL_SIZE = size_x * size_y;
			//const size_t TOTAL_BLOCKS_COUNT = TOTAL_SIZE / info.L1_size;

			
			//const size_t HEIGHT = CACHE_LINE_SIZE_IN_BYTES;
			//const size_t WIDTH = info.L1_size / HEIGHT;

			//this->columns = X_SIZE_IN_BYTES / WIDTH;
			//this->rows = Y_SIZE_IN_BYTES / HEIGHT;
			



			

			//size_t size_y = ELEMENTS_COUNT_IN_L1_CACHE / CACHE_LINE_SIZE_IN_BYTES;



			//size_t totalSize = size_x * sizeof(T) * size_y;

			//size_t full_part = totalSize / info.L1_size;
			//size_t division_part = (bool)ceil(totalSize % info.L1_size);
			//size_t matrix_parts_count = full_part + division_part;

			//block_size = totalSize / matrix_parts_count;
			//arrays.reserve(matrix_parts_count);

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