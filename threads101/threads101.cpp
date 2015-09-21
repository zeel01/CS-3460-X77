// threads101.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>

#include "cs477.h"


void function(void *context)
{
//	std::cout << "hello, world" << std::endl;
	printf("hello, world\n");
}

#include <ppl.h>
#include <ppltasks.h>

int main()
{
	
	std::atomic<int> sum = 0;

	for (int T = 2; T < (1 << 16); T *= 2)
	{
		std::vector<cs477::future> futures;
		futures.reserve(T);

		std::vector<cs477::thread> threads;
		threads.reserve(T);

		try
		{
			sum = 0;
			{
				auto t1 = now();

				for (int i = 0; i < T; i++)
				{
					auto f = cs477::queue_work([&sum, i]
					{
						sum += i;
					});

					futures.push_back(std::move(f));
				}

				for (auto &f : futures)
				{
					f.wait();
				}

				auto t2 = now();
				printf("%d: %d, %g, ", T, sum.load(), to_seconds(t1, t2));
			}

			sum = 0;
			{
				auto t1 = now();

				for (int i = 0; i < T; i++)
				{
					auto t = cs477::create_thread([&sum, i]
					{
						sum += i;
					});

					threads.push_back(t);
				}

				for (auto &t : threads)
				{
					cs477::join(t);
				}

				auto t2 = now();
				printf("%g\n", to_seconds(t1, t2));

			}
		}
		catch (std::system_error &ex)
		{
			printf("Error: %d\n", ex.code().value());
		}
		catch (std::exception &ex)
		{
			printf("Error: %s\n", ex.what());
		}
		catch (...)
		{
			printf("Error!\n");
		}
	}

}
