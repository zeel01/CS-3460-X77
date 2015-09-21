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
	for (int i = 2; i < 100000000; i++)
	{
		if (is_prime(i))
		{
			std::cout << i << std::endl;
		}
	}
    return 0;
}

