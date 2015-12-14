// primes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include "cs477.h"



bool is_prime(int n)
{
	auto j = static_cast<int>(sqrt(n));
	for (int i = 2; i <= j; i++)
	{
		if (n % i == 0) return false;
	}
	return true;
}


int main()
{
	int maxThreads = 16;
	int target = 100000000;

	std::vector<cs477::thread> threads;
	threads.reserve(maxThreads);

	cs477::vector<int>* primes = new cs477::vector<int>[maxThreads];

	int chunkSize = target / maxThreads;
	primes[0].push_back(2);

	auto start = now();

	for (int i = 0; i < maxThreads; i++) {
		auto t = cs477::create_thread([&primes, chunkSize, i] {
			int start = chunkSize * i + 1;
			for (int n = start; n < start + chunkSize; n += 2) {
				if (is_prime(n)) primes[i].push_back(n);
			}
		});

		threads.push_back(t);
	}

	for (auto &t : threads) {
		cs477::join(t);
	}

	auto time = to_seconds(start, now());

	std::cout << "Total Time: " << time << std::endl;

	for (int i = 0; i < maxThreads; i++) {
		for (auto t : primes[i]) std::cout << t << std::endl;
	}

	std::cout << "Total Time: " << time << std::endl;

    return 0;
}

