#pragma once
#include <cassert>


#include "../ForBenchmarks/CG_base.hpp"
#include "../ForBenchmarks/Flat/CG_flat_sum.hpp"
#include "../ForBenchmarks/Flat/CG_flat_sse.hpp"


import FlatArray;

import std.compat;


using std::printf;

namespace Test {
	bool testFlatArray() {
		constexpr size_t width = 20, height = 5, capacity = width * height;
		constexpr bool writeToStdout = false;

		Flat2DArray<float> array = Flat2DArray<float>(width, height);

		for (size_t x = 0; x < capacity; x+=2) {
			array[x] = (float)x;
			array[x + 1] = (float)(x + 1);
		}


		int counter = width - 1;
		for (size_t x = 0; x < capacity; x++, counter--) {
			float data = array[x];
			if (writeToStdout) printf("%2.f ", data);
			if (counter == 0) {
				if (writeToStdout) printf("\n");
				counter = width;
			}

			if (data != x) {
				if (writeToStdout) printf("ERROR testFlatArray(): ошибка при тестировании плоского массива! .at(x)!\n");
				return false;
			}
		}


		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				float data = array.at(x, y);
				if (writeToStdout) printf("%2.f ", data);

				if (data != (x + y * width)) {
					if (writeToStdout) printf("ERROR testFlatArray(): ошибка при тестировании плоского массива .at(x, y)!\n");
					return false;
				}
			}
			if (writeToStdout) printf("\n");
		}	


		return true;
	}

	bool testAllRealizationsIsRight() {
		using std::vector;

		constexpr size_t width = 7, height = 5, capacity = width * height;
		constexpr int threadsCount = 2;

		std::array<bool, capacity> idealArray ={
			0, 1, 1, 1, 1, 1, 0, 
			1, 0, 0, 0, 0, 0, 1, 
			1, 0, 0, 0, 0, 0, 1, 
			1, 0, 0, 0, 0, 0, 1, 
			0, 1, 1, 1, 1, 1, 0
		};

		vector<bool> vec{};
		vec.resize(capacity);

		// заполнение массива данными
		for (size_t x = 0; x < width; x++) {
			for (size_t y = 0; y < height; y++) {
				vec[x + y * width] = (x + y % 4) % 2 + !(x + 1 != 3);
			}
		}

		auto printBoolPtr = [&](size_t capacity, size_t width, bool* ptr) {
			int row = int(width - 1);
			for (size_t x = 0; x < capacity; x++, row--) {
				printf("%d ", ptr[x]);
				if (row == 0) { printf("\n"); row = (int)width; }
			}
			return;
		};
		auto printBoolVec = [&](size_t capacity, size_t width, vector<bool>& ptr) {
			int row = int(width - 1);
			for (size_t x = 0; x < capacity; x++, row--) {
				printf("%d ", (int)ptr[x]);
				if (row == 0) { printf("\n"); row = (int)width; }
			}
			return;
		};

		auto isResultEqual = [&](bool* arr) -> bool {
			for (size_t x = 0; x < capacity; x++) {
				if (idealArray[x] != arr[x]) return false;
			}
			return true;
		};

		printf("Source array:\n");
		printBoolVec(capacity, width, vec);
		printf("\n");

		printf("Expected Array:\n");
		printBoolPtr(capacity, width, idealArray.data());

		using std::shared_ptr, std::make_shared;

		vector<shared_ptr<CaveGeneratorBench>> cavesTICK{};
		vector<shared_ptr<CaveGeneratorBench>> cavesTICKMT{};
		constexpr int CAVES_COUNT = 5;
		cavesTICK.reserve(CAVES_COUNT);

		cavesTICK.push_back(make_shared<CaveGenerator_flat_sum>(width, height, vec, threadsCount));
		cavesTICK.push_back(make_shared<CaveGenerator_flat_sse>(width, height, vec, threadsCount));

		cavesTICKMT = cavesTICK;

		for (auto& cave : cavesTICK) {
			cave->Tick();
			if (isResultEqual(cave->Data()) == false) {
				printf("Tick [%s] Tick unsuccessful\n", cave->getName().c_str());
			}
		}

		for (auto& cave : cavesTICKMT) {
			cave->TickMT();
			if (isResultEqual(cave->Data()) == false) {
				printf("TickMT [%s] TickMT unsuccessful\n", cave->getName().c_str());
			}
		}
		

		return true;
	}

	void Run() {
		assert(testFlatArray() == true && "Плоский массив не прошёл тест"); printf("Flat2DArray passed\n");
		assert(testAllRealizationsIsRight() == true && "Реализации не прошли тест на правильность тиков");
		return;
	}
}