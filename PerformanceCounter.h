#pragma once

//Class to measure time in milliseconds.


#ifdef _WIN32
class PerformanceCounter
{
	double freq;
	double start_time;
public:
	PerformanceCounter() = default;

	void start()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		//Record as milliseconds
		freq = double(li.QuadPart) / 1000.0;

		QueryPerformanceCounter(&li);
		start_time = double(li.QuadPart);
	}

	double getCounter() const
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart - start_time) / freq;
	}

	double getAndReset()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		const double result = (li.QuadPart - start_time) / freq;
		start_time = double(li.QuadPart);
		return result;
	}


};

#else
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
	
class PerformanceCounter
{
	std::chrono::time_point<Clock> start_time;
public:
	PerformanceCounter() = default;

	void start()
	{
		start_time = Clock::now();
	}

	double getCounter() const
	{
		std::chrono::time_point<Clock> end = Clock::now();
		std::chrono::duration<double> diff = end-start_time;
		return diff.count() * 1000.0;
	}

	
	double getAndReset()
	{ 
		std::chrono::time_point<Clock> end = Clock::now();
		std::chrono::duration<double> diff = end-start_time;
		double result = diff.count() * 1000.0;
		start_time = end;
		return result;
	}
};

#endif


