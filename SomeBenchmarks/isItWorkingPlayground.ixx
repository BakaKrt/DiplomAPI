export module isItWorkingPlayground;

import std;

import sse_vertical;
import sse_horizontal;
import iter;
import normalsumv2;
import avx_horizontal;

import normalRule;
import sseRule;
import avxRule;
import bufferedRule;
import bitsetRule;
import bitsetBufferedRule;

import TotalNaiveRealization;
import OnlyAvxRealization;
import AvxBufferedRealization;

import random;

using std::size_t;
using std::cout, std::printf;

#ifdef _DEBUG
export inline void playgroundTest0() {
	IterSum horizontal {}; SSEv1Sum vertical {};
	size_t width = 14*2 + 1, height = 4, capacity = width * height;

	size_t minimal = (width < 16) ? width : 16;

	auto mem = Flat2DArray<uint8_t>(width, height, 16);

	for (size_t i = 0; i < capacity; i++) {
		mem[i] = (uint8_t) (i % 11);
	}

	cout << "init mem width: " << width << " height: " << height << "\n";

	mem._debug_print_as_arrays(minimal);


	auto hor_res = horizontal.test_run(mem);
	cout << "normalRes:\n"; hor_res._debug_print_as_arrays(minimal);

	auto vert_res = vertical.test_run(mem);
	cout << "res:\n"; vert_res._debug_print_as_arrays(minimal);
	cout << "normalRes:\n"; hor_res._debug_print_as_arrays(minimal);
	
	for (size_t x = 0; x < vert_res.capacity(); x++) {
		if (vert_res[x] != hor_res[x]) {
			printf("got diff index[%3u]: v %u i %u\n", (unsigned) x, vert_res[x], hor_res[x]);
		}
	}
}

export inline void playgroundTest3() {
	SSEv2Sum horizontal {}; AVXv1HorizontalSum vertical {};
	size_t width = 36, height = 10, capacity = width * height;

	size_t minimal = (width < 16) ? width : 16;

	auto mem = Flat2DArray<uint8_t>(width, height, 16);

	for (size_t i = 0; i < capacity; i++) {
		mem[i] = (uint8_t) (i % 11);
	}

	cout << "init mem width: " << width << " height: " << height << "\n";

	mem._debug_print_as_arrays(minimal);

	auto vert_res = vertical.test_run(mem);
	cout << "res:\n"; vert_res._debug_print_as_arrays(minimal);

	auto hor_res = horizontal.test_run(mem);
	cout << "normalRes:\n"; hor_res._debug_print_as_arrays(minimal);
	cout << "res:\n"; vert_res._debug_print_as_arrays(minimal);
	
	cout << "finded bug, with width %16 sseHorizontal calc with error!!!!!!\n";

	bool isEqual = true;

	for (size_t x = 0; x < vert_res.capacity(); x++) {
		if (vert_res[x] != hor_res[x]) {
			printf("got diff index[%3u]: v %u h %u\n", (unsigned) x, vert_res[x], hor_res[x]); isEqual = false;
		}
	}
	if (isEqual) printf("arrays are equal\n");
}

export inline void playgroundTest4() {
	NormalWOAllocSum horizontal {}; IterSum vertical {};
	size_t width = 16, height = 5, capacity = width * height;

	size_t minimal = (width < 16) ? width : 16;

	auto mem = Flat2DArray<uint8_t>(width, height, 16);

	for (size_t i = 0; i < capacity; i++) {
		mem[i] = (uint8_t) (i % 11);
	}

	cout << "init mem width: " << width << " height: " << height << "\n";

	mem._debug_print_as_arrays(minimal);

	auto vert_res = vertical.test_run(mem);
	cout << "res:\n"; vert_res._debug_print_as_arrays(minimal);

	auto hor_res = horizontal.test_run(mem);
	cout << "normalRes:\n"; hor_res._debug_print_as_arrays(minimal);
	cout << "res:\n"; vert_res._debug_print_as_arrays(minimal);
	
	cout << "finded bug, with width %16 sseHorizontal calc with error!!!!!!\n";

	for (size_t x = 0; x < vert_res.capacity(); x++) {
		if (vert_res[x] != hor_res[x]) {
			printf("got diff index[%3u]: v %u h %u\n", (unsigned) x, vert_res[x], hor_res[x]);
		}
	}
}

export inline void playgroundTest01() {
	IterSum horizontal {}; SSEv1Sum vertical {};
	size_t height = 4, capacity = 0;

	using std::vector;

	vector<size_t> widths {16, 17};
	for (size_t width = 18; width < 256; width++) {
		widths.push_back(width);
	}
	vector<bool> results{}; results.reserve(widths.size());

	for (auto& width : widths) {
		auto mem = Flat2DArray<uint8_t>(width, height, 16);

		capacity = width * height;

		for (size_t i = 0; i < capacity; i++) {
			mem[i] = (uint8_t)(i % 11);
		}

		auto hor_res = horizontal.test_run(mem);
		auto vert_res = vertical.test_run(mem);

		bool isEqual = true;

		for (size_t x = 0; x < vert_res.capacity(); x++) {
			if (vert_res[x] != hor_res[x]) {
				isEqual = false;
				printf("got diff index[%3u]: v %u i %u\n", (unsigned)x, vert_res[x], hor_res[x]);
			}
		}
		results.push_back(isEqual);
	}

	for (size_t x = 0; x < results.size(); x++) {
		if (results[x] == false) {
			std::cout << "finded difference on width = " << widths[x] << "!\n";
		}
	}
}

export inline void playgroundTest1() {
	AvxRule avxRule {}; BitsetRule BitsetBuffered {};
	size_t width = 37, height = 1, capacity = width * height;

	auto originalArray = generateVectorOfAlignedMemoryForGameOfLife(width, height, 1, 32, false)[0];
	auto bitbufNeighbours = generateVectorOfAlignedMemoryForGameOfLife(width, height, 1, 32, true)[0];

	auto avxNeighbours(bitbufNeighbours);

	cout << "neigh:\n" << bitbufNeighbours << "\n";
	cout << "ish:\n" << originalArray << "\n";


	BitsetBuffered.applyRule(originalArray, bitbufNeighbours);
	cout << "res bitset:\n" << bitbufNeighbours << "\n";

	avxRule.applyRule(originalArray, avxNeighbours);
	cout << "res avx:\n" << avxNeighbours << "\n";

	for (size_t x = 0; x < capacity; x++) {
		uint8_t& bitVal = bitbufNeighbours[x];
		uint8_t& avxVal = avxNeighbours[x];
		if (bitVal != avxVal) {
			printf("there a miss: %zu, values: %2u %2u\n", x, bitVal, avxVal);
		}
	}
}

export inline void playgroundTestTotal() {
	TotalOnlyAvxRealization optimized {};  NaiveRealization naive{};

	constexpr size_t width = 32*2 + 5, height = 5, iterations = 1;
	constexpr size_t alignment = 32;

	auto originalArray = generateVectorOfAlignedMemoryForGameOfLife(width, height, iterations, alignment, false);
	auto neigboursArray = generateVectorOfAlignedMemoryForGameOfLife(width, height, iterations, alignment, true);

	auto neighbours_copy(neigboursArray);

	bool isEqual = true;

	for (size_t i = 0; i < iterations; i++) {
		naive.run(originalArray[i], neigboursArray[i]);
		optimized.run(originalArray[i], neighbours_copy[i]);


		for (size_t count = 0; count < width * height; count++) {
			uint8_t& naiveVal = neigboursArray[i][count];
			uint8_t& optVal = neighbours_copy[i][count];
			if (naiveVal != optVal) {
				printf("there a miss: %zu, values: %2u %2u\n", count, naiveVal, optVal);
				isEqual = false;
			}
		}
		std::cout << "naive:\n"; neigboursArray[i]._debug_print_as_arrays(32);
		std::cout << "optim:\n"; neighbours_copy[i]._debug_print_as_arrays(32);
		std::cout << " orig:\n"; originalArray[i]._debug_print_as_arrays(32);
	}

	if (isEqual) printf("arrays are equal ~ SSE vs Normal rules\n");

	return;
}

export inline void playgroundTestFilters() {
	BitsetBufferedRule buffered{}; NormalRule normal{};

	constexpr size_t width = 39, height = 2, iterations = 1;
	constexpr size_t alignment = 8;

	auto originalArray = generateVectorOfAlignedMemoryForGameOfLife(width, height, iterations, alignment, false);
	auto neigboursArray = generateVectorOfAlignedMemoryForGameOfLife(width, height, iterations, alignment, true);

	auto neighbours_copy(neigboursArray);

	bool isEqual = true;

	for (size_t i = 0; i < iterations; i++) {
		buffered.applyRule(originalArray[i], neigboursArray[i]);
		normal.applyRule(originalArray[i], neighbours_copy[i]);

		for (size_t index = 0; index < width * height; index++) {
			bool localEqual = true;
			uint8_t& bufVal = neigboursArray[i][index];
			uint8_t& normalVal = neighbours_copy[i][index];
			if (bufVal != normalVal) {
				printf("there a miss: %zu, values: %2u %2u\n", index, bufVal, normalVal);
				isEqual = false;
				localEqual = false;
			}
		}
		std::cout << neigboursArray[i] << "\n" << neighbours_copy[i];
	}

	if (isEqual) printf("arrays are equal ~ Buffered vs Normal rules\n");

	return;
}
#endif // _DEBUG	