#include "Random.h"

std::random_device RandomGenerator::m_rd;
std::mt19937 RandomGenerator::m_gen(m_rd());


size_t RandomGenerator::getRandomNumber(size_t max)
{
	if (max == 0)
		return 0;
	std::uniform_int_distribution<size_t> distr(0, max);
	return distr(m_gen);
}