#pragma once
#include "Random.h"

Random::Random()
{
	gen.seed(rd());
}

void Random::init()
{
	gen.seed(rd());
}

int Random::randi(int max)
{
	const std::uniform_int_distribution<int> distribution(0, max);
	const int result = distribution(gen);
	return result;
}

unsigned int Random::randu(unsigned max)
{
	const std::uniform_int_distribution<unsigned> distribution(0, max);
	const unsigned int result = distribution(gen);
	return result;
}

double Random::randd(const double min, const double max)
{
	const std::uniform_real_distribution<double> distribution(min, max);
	const double result = distribution(gen);
	return result;
}

float Random::randf(const float min, const float max)
{
	const std::uniform_real_distribution<float> distribution(min, max);
	const float result = distribution(gen);
	return result;
}

std::random_device Random::rd;
std::mt19937 Random::gen;