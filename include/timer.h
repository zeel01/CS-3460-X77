
#pragma once

#include <chrono>

inline auto now()
{
	return std::chrono::high_resolution_clock::now();
}

inline auto to_seconds(
	std::chrono::time_point<std::chrono::high_resolution_clock> t1, 
	std::chrono::time_point<std::chrono::high_resolution_clock> t2)
{
	std::chrono::duration<double> diff = t2 - t1;
	return diff.count();
}



std::pair<double, double> get_process_times()
{
	union time
	{
		FILETIME ft;
		LARGE_INTEGER li;
	};
	time c, e, k, u;
	GetProcessTimes(GetCurrentProcess(), &c.ft, &e.ft, &k.ft, &u.ft);

	SYSTEMTIME sk, su;
	FileTimeToSystemTime(&k.ft, &sk);
	FileTimeToSystemTime(&u.ft, &su);

	double kernel = static_cast<double>(k.li.QuadPart) / 10000000.0;
	double user = static_cast<double>(u.li.QuadPart) / 10000000.0;

	return std::make_pair(kernel, user); 
}


