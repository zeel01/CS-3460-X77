// threads101.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>

#include "..\inlcude\thread.h"


void function(void *context)
{
	std::cout << "hello, world" << std::endl;
//	printf("hello, world\n");
}

int main()
{
	auto thread = create_thread(function, nullptr);	

    return 0;
}

















//int main()
//{
//	std::vector<thread> threads;
//
//	try
//	{
//		for (int i = 0; i < 100000; i++)
//		{
//			threads.push_back(create_thread([i]
//			{
//				Sleep(100);
//				std::cout << i << std::endl;
//				//printf("%d\n", i);
//			}));
//		}
//
//		for (auto &&thread : threads)
//		{
//			join(thread);
//		}
//	}
//	catch (std::system_error &ex)
//	{
//		printf("Error: %d\n", ex.code().value());
//	}
//	catch (std::exception &ex)
//	{
//		printf("Error: %s\n", ex.what());
//	}
//	catch (...)
//	{
//		printf("Error!\n");
//	}
//
//}
