#pragma once

#include <random>
#include <vector>

class RandomGenerator {
public:

	static size_t getRandomNumber(size_t max);

private:
	static std::random_device m_rd;
	static std::mt19937 m_gen;
};

