

#pragma once

#include "cs477.h"

#include <algorithm>
#include <thread>

namespace cs477
{

	template<class InputIt, class T>
	size_t count(InputIt first, InputIt last, const T& value)
	{

		if (last - first < 5000)
		{
			// Let the STL handle the small cases
			return std::count(first, last, value);
		}

		// Parition the work into n chunks.  
		// Each thread will work only on one chunk.
		auto threads = std::thread::hardware_concurrency();
		auto count = last - first;
		auto count_per_thread = count / threads;

		std::vector<future<size_t>> futures;

		auto chunk_first = first;
		for (unsigned i = 0; i < threads; i++)
		{
			auto chunk_last = chunk_first + count_per_thread;

			futures.push_back(queue_work([chunk_first, chunk_last, value]
			{
				// Compute the count of this chunk.
				size_t ret = 0;
				auto first = chunk_first;
				for (; first != chunk_last; ++first)
				{
					if (*first == value)
					{
						ret++;
					}
				}

				return ret;
			}));

			// Don't forget to move to the next chunk
			chunk_first = chunk_last;
		}

		// Wait for all the futures to finish.
		auto f = when_all<size_t>(futures.begin(), futures.end());
		auto counts = f.get();
		
		// Compute the sum of the local values.
		size_t ret = 0;
		for (auto i : counts)
		{
			ret += i;
		}

		return ret;
	}

}

