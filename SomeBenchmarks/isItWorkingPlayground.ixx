export module isItWorkingPlayground;

import std;

import sse_vertical;
import sse_horizontal;
import iter;

import normalRule;
import sseRule;

import random;

using std::size_t;
using std::cout, std::printf;

#ifdef _DEBUG
export inline void playgroundTest0() {
	IterSum horizontal {}; SSEv1Sum vertical {};
	size_t width = 14*2 + 1, height = 4, capacity = width * height;

	size_t minimal = (width < 16) ? width : 16;

	auto mem = Flat2DArray<uint8_t>(width, height, 16, false);

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
	SSEv2Sum horizontal {}; IterSum vertical {};
	size_t width = 17, height = 5, capacity = width * height;

	size_t minimal = (width < 16) ? width : 16;

	auto mem = Flat2DArray<uint8_t>(width, height, 16, false);

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
		auto mem = Flat2DArray<uint8_t>(width, height, 16, false);

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
	SseRule sseRule {}; NormalRule normRule {};
	size_t width = 17, height = 1, capacity = width * height;
	auto original_mem = Flat2DArray<uint8_t>(width, height, 16, false);
	auto neighbours_mem_0 = Flat2DArray<uint8_t>(width, height, 16, false);
	auto neighbours_mem_1 = Flat2DArray<uint8_t>(width, height, 16, false);

	for (size_t x = 0; x < capacity; x++) {
		original_mem[x] = uint8_t(x % 2);

		auto random_value = randomUint8(0, 5);

		neighbours_mem_0[x] = random_value;
		neighbours_mem_1[x] = random_value;
	}

	cout << "ish normal:\n" << neighbours_mem_0 << "\n";
	cout << "ish sse:\n" << neighbours_mem_1 << "\n";


	normRule.applyRule(original_mem, neighbours_mem_0);
	cout << "res normal:\n" << neighbours_mem_0 << "\n";

	sseRule.applyRule(original_mem, neighbours_mem_1);
	cout << "res sse:\n" << neighbours_mem_1 << "\n";

	for (size_t x = 0; x < capacity; x++) {
		if (neighbours_mem_0[x] != neighbours_mem_1[x]) {
			printf("got diff index[%3u]: 0: %u  1: %u\n", (unsigned) x, neighbours_mem_0[x], neighbours_mem_1[x]);
		}
	}
}
#endif // _DEBUG	