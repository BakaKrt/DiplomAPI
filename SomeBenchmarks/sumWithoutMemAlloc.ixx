export module normalsumv2;

import std;
import sumRealizationBase;

using std::string;
using std::array;

export class NormalWOAllocSum : public SumRealizationBase<NormalWOAllocSum> {
public:
	NormalWOAllocSum() { name = "norm v2"; }

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


		T* ptr = object.data();
		T* secondPtr = to_save.data();
		const size_t width = object.width();
		const size_t height = object.height();
		const size_t capacity = width * height;


		auto GNTop = [&] () noexcept {
			for (size_t i = 1; i < width - 1; i++) {
				secondPtr[i] = ptr[i - 1] + ptr[i + 1] + ptr[i - 1 + width] + ptr[i + width] + ptr[i + 1 + width];
			}
		};
		auto GNBot = [&] () noexcept {
			for (size_t i = capacity - width + 1; i < capacity - 1; i++) {
				secondPtr[i] = ptr[i - 1 - width] + ptr[i - width] + ptr[i + 1 - width] + ptr[i - 1] + ptr[i + 1];
			}
		};

		auto GNLeft = [&] () noexcept {
			size_t y_offset = 0;
			for (size_t y = 1; y < height - 1; y++) {
				y_offset = y * width;
				secondPtr[y_offset] = ptr[y_offset - width] + ptr[y_offset - width + 1] + ptr[y_offset + 1] + ptr[y_offset + width] + ptr[y_offset + width + 1];
			}
			};
		auto GNRight = [&] () noexcept {
			const size_t x = width - 1;
			size_t y_offset = 0;
			for (size_t y = 1; y < height - 1; y++) {
				y_offset = y * width;
				secondPtr[y_offset + x] = ptr[y_offset - width + x - 1] + ptr[y_offset - width + x] + ptr[y_offset + x - 1] + ptr[y_offset + width + x - 1] + ptr[y_offset + width + x];
			}
		};

		auto GNTopLeft = [&ptr, width, &secondPtr] () noexcept { secondPtr[0] = ptr[1] + ptr[width] + ptr[width + 1]; };
		auto GNTopRight = [&ptr, width, &secondPtr] () noexcept { secondPtr[width - 1] = ptr[width - 2] + ptr[2 * width - 2] + ptr[2 * width - 1]; };
		auto GNBotLeft = [&ptr, width, capacity, height, &secondPtr] () noexcept {	secondPtr[width * height - width] = ptr[capacity - 2 * width] + ptr[capacity - 2 * width + 1] + ptr[capacity - width + 1]; };
		auto GNBotRight = [&ptr, width, capacity, height, &secondPtr] () noexcept { secondPtr[width * height - 1] = ptr[capacity - width - 2] + ptr[capacity - width - 1] + ptr[capacity - 2]; };

		auto GetNeighbours = [&width, &ptr, &secondPtr] (size_t from_offset, size_t save_offset) {
			T* _ptr = ptr + from_offset;

			secondPtr[save_offset] = _ptr[0] + _ptr[1] + _ptr[2] + \
				_ptr[width] + _ptr[width + 2] + \
				_ptr[width * 2] + _ptr[width * 2 + 1] + _ptr[width * 2 + 2];
		};

		GNTopLeft();
		GNTop();
		GNTopRight();
		GNLeft();

		for (size_t y = 1; y < height - 1; y++) {
			size_t top_left_y_offset = width * (y - 1);
			size_t y_offset = y * width;
			for (size_t x = 1; x < width - 1; x++) {
				GetNeighbours(top_left_y_offset + x - 1, y_offset + x);
			}
		}

		GNRight();
		GNBotLeft();
		GNBot();
		GNBotRight();
	}
};
