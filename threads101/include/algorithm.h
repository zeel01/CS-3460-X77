

#pragma once

#include "cs477.h"

#include <algorithm>
#include <thread>

namespace cs477
{

	//template<class InputIt, class T>
	//size_t count(InputIt first, InputIt last, const T& value)
	//{

	//	if (last - first < 5000)
	//	{
	//		return std::count(first, last, value);
	//	}

	//	auto threads = std::thread::hardware_concurrency();
	//	auto count = last - first;
	//	auto count_per_thread = count / threads;

	//	std::vector<future> futures;
	//	std::vector<size_t> counts;
	//	counts.resize(threads);

	//	auto chunk_first = first;
	//	for (unsigned i = 0; i < threads; i++)
	//	{
	//		auto chunk_last = chunk_first + count_per_thread;

	//		auto f = queue_work([i, &counts, chunk_first, chunk_last, value]
	//		{
	//			size_t ret = 0;
	//			auto first = chunk_first;
	//			for (; first != chunk_last; ++first)
	//			{
	//				if (*first == value)
	//				{
	//					ret++;
	//				}
	//			}

	//			counts[i] = ret;
	//		});

	//		chunk_first = chunk_last;

	//		futures.push_back(std::move(f));
	//	}

	//	for (auto &&f : futures)
	//	{
	//		f.wait();
	//	}

	//	size_t ret = 0;
	//	for (auto i : counts)
	//	{
	//		ret += i;
	//	}

	//	return ret;
	//}

}

