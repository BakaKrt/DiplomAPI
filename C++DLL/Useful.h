#pragma once

#include <random>
#include <cstdint>

using byte = uint8_t;

static float* RandomFloatArray(const size_t len, const float l, const float r)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(l, r);

	float* ptr = new float[len];
	for (size_t pos = 0; pos < len; pos++) {
		ptr[pos] = dist(gen);
	}

	return ptr;
}

static float* RandomFloatCanonical(const size_t len)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.0, 1.0);

	float* ptr = new float[len];
	for (size_t pos = 0; pos < len; pos++) {
		float rand = dist(gen);
		ptr[pos] = rand;
	}

	return ptr;
}

static byte* RandomByteArray(const size_t size, const byte l, const byte r)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(l, r);

	byte* ptr = new byte[size];
	for (size_t pos = 0; pos < size; pos++) {
		ptr[pos] = (byte)dist(gen);
	}

	return ptr;
}