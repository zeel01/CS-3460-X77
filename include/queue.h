
#pragma once

#include "cs477.h"


namespace cs477
{
	template <typename T>
	class queue
	{
	public: 
		queue();

		queue(queue &&) = delete;
		queue(const queue &) = delete;

		queue &operator =(queue &&) = delete;
		queue &operator =(const queue &&) = delete;

	public:
		void push_back(T &&t);
		void push_back(const T &t);
		
		T pop_back();

	public:
		std::list<T> list;
		mutex mtx;
		condition_variable cv;
	};


}