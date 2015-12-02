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


void threads_vs_threadpools();

void concurrent_vector();
void concurrent_queue();


#include "algorithm.h"

int main()
{

	//std::vector<cs477::future<int>> futures;
	//for (int i = 0; i < 100; i++)
	//{
	//	futures.push_back(cs477::queue_work([i]
	//	{
	//		return i * i;
	//	}));
	//}

	//auto result = cs477::when_all<int>(futures.begin(), futures.end());



	std::vector<int> v;
	v.resize(1000000);

	auto count = cs477::parallel_count(v.begin(), v.end(), 0);
	printf("%d\n", count);

	auto sum = cs477::parallel_sum(v.begin(), v.end());
	printf("%d\n", sum);

	return 0;
}




void threads_vs_threadpools()
{
	std::atomic<int> sum = 0;

	for (int T = 2; T < (1 << 16); T *= 2)
	{
		std::vector<cs477::future<void>> futures;
		futures.reserve(T);

		std::vector<cs477::thread> threads;
		threads.reserve(T);


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
}





void concurrent_vector()
{
	cs477::vector<std::string> list;

	std::vector<cs477::future<void>> futures;

	for (int i = 0; i < 200; i++)
	{
		auto f = cs477::queue_work([&list, i]
		{
			char buf[50];
			sprintf_s(buf, "Hello %d (%d)", i, GetThreadId(GetCurrentThread()));
			list.push_back(buf);
		});

		futures.push_back(std::move(f));
	}

	for (auto &f : futures)
	{
		f.wait();
	}

	for (auto &i : list)
	{
		printf("%s\n", i.c_str());
	}

}



void concurrent_queue()
{
	cs477::queue<std::string> queue;
	std::vector<cs477::future<void>> futures;

	for (int i = 0; i < 200; i++)
	{
		auto f = cs477::queue_work([&queue, i]
		{
			char buf[50];
			sprintf_s(buf, "Hello %d (%d)", i, GetThreadId(GetCurrentThread()));
			queue.push(buf);
		});

		futures.push_back(std::move(f));
	}

	for (;;)
	{
		auto str = queue.pop();
		if (str.empty())
		{
			break;
		}
		printf("%s\n", str.c_str());
	}

}